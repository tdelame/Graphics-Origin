# ifndef GRAPHICS_ORIGIN_MEMORY_H_
# define GRAPHICS_ORIGIN_MEMORY_H_

# include "./assert.h"
# include "./log.h"
# include <new>
/**
 * The design comes from the Molecule Engine, explained by Stefan Reinalter
 * on his blog:
 * https://blog.molecular-matters.com/2011/08/03/memory-system-part-5/
 */
namespace graphics_origin {
  namespace tools {

# define go_new_align(type_,alignement_,allocator_) new (allocator_.allocate( \
    sizeof(type_),                                                            \
    alignement_,                                                              \
    __FILE__,                                                                 \
    GO_STRINGIZE(__LINE__),                                                   \
    __FUNCTION__)) type_

# define go_new(type_,allocator_) go_new_align(type_,alignof(type_),allocator_)

# define go_delete(object_,allocator_) graphics_origin::tools::detail::deallocate( object_, allocator_ )

# define go_new_array_align(type_,alignment_,allocator_)                      \
    graphics_origin::tools::detail::new_array< std::remove_all_extents<type_>::type>(\
      allocator_,                                                             \
      std::extent<type_,0>::value,                                            \
      alignment_,                                                             \
      __FILE__,                                                               \
      GO_STRINGIZE(__LINE__),                                                 \
      __FUNCTION__,                                                           \
      std::integral_constant<bool,std::is_pod<type_>::value>() )

# define go_new_array(type_,allocator_) go_new_array_align(type_,alignof(type_),allocator_)

# define go_delete_array(object_,allocator_) graphics_origin::tools::detail::delete_array( object_, allocator_ )

    namespace detail {
      template< typename type, class allocator >
      inline void deallocate( type* ptr, allocator& a )
      {
        ptr->~type();
        a.deallocate( ptr );
      }

      template< typename type, class allocator >
      inline type* new_array( allocator& a, const size_t& N, size_t alignment,
          const char* file, const char* line, const char* function,
          std::true_type )
      {
        return (type*)a.allocate( N, alignment, file, line, function );
      }

      template< typename type, class allocator >
      inline type* new_array( allocator& a, const size_t& N, size_t alignment,
          const char* file, const char* line, const char* function,
          std::false_type )
      {
        union {
          void* as_void;
          size_t* as_size_t;
          type* as_type;
        };

        as_void = a.allocate(sizeof(type)*N + sizeof(size_t), alignment, file, line, function );

        // store number of instances in first size_t bytes
        *as_size_t++ = N;

        // construct instances using placement new
        const type* const onePastLast = as_type + N;
        while (as_type < onePastLast)
          new (as_type++) type;

        // hand user the pointer to the first instance
        return (as_type - N);
      }

      template< typename type, class allocator >
      inline void delete_array( type* ptr, allocator& a, std::true_type )
      {
        a.deallocate( ptr );
      }

      template< typename type, class allocator >
      inline void delete_array( type* ptr, allocator& a, std::false_type )
      {
        union {
          size_t* as_size_t;
          type* as_type;
        };

        // user pointer points to first instance...
        as_type = ptr;

        // ...so go back size_t bytes and grab number of instances
        const size_t N = as_size_t[-1];

        // call instances' destructor in reverse order
        for (size_t i=N; i>0; --i)
          as_type[i-1].~T();

        a.deallocate(as_size_t-1);
      }

      template< typename type, class allocator >
      inline void delete_array( type* ptr, allocator& a )
      {
        delete_array( ptr, a, std::integral_constant<bool,std::is_pod<type>::value>());
      }
    }

    template<
      class allocation_policy,
      class thread_policy,
      class bounds_checking_policy,
      class memory_tracking_policy,
      class memory_tagging_policy >
    class memory_arena {
    public:
      explicit memory_arena( allocation_policy* allocation )
        : allocator{ allocation }
      {}

      void* allocate(
          size_t size,
          size_t alignment,
          const char* file,
          const char* line,
          const char* function )
      {
        thread_guard.enter();
          const size_t original_size = size;
          const size_t new_size = size + bounds_checking_policy::size_front + bounds_checking_policy::size_back;
          char* plain_memory = static_cast<char*>( allocator->allocate( new_size, alignment, bounds_checking_policy::size_front));

          bounds_checker.guard_front( plain_memory );
          memory_tagger.tag_allocation( plain_memory + bounds_checking_policy::size_front, original_size );
          bounds_checker.guard_back( plain_memory + bounds_checking_policy::size_front + original_size );

          memory_tracker.track( plain_memory, new_size, alignment, file, line, function );

        thread_guard.leave();

        return plain_memory + bounds_checking_policy::size_front;
      }

      void deallocate( void* ptr )
      {
        thread_guard.enter();

          char* original_memory = static_cast<char*>( ptr ) - bounds_checking_policy::size_front;
          const size_t original_size = allocator->get_allocation_size( original_memory );

          bounds_checker.check_front( original_memory );
          bounds_checker.check_back( original_memory + original_size - bounds_checking_policy::size_back );
          memory_tracker.untrack( original_memory );
          memory_tagger.tag_deallocation( original_memory, original_size );

          allocator->deallocate( original_memory );
        thread_guard.leave();
      }

    private:
      allocation_policy* allocator;
      thread_policy thread_guard;
      bounds_checking_policy bounds_checker;
      memory_tracking_policy memory_tracker;
      memory_tagging_policy memory_tagger;
    };

    namespace allocation_policy {
      /**@brief Implements a linear allocator.
       *
       * A linear allocator cannot free individual allocations. Instead, it
       * frees them all when calling reset.
       *
       *
       */
      class linear {
      public:
        static constexpr size_t size_front = sizeof(uint32_t);

        /**@brief Construct a new linear allocator.
         *
         * Instantiate a new linear allocator on an area of contiguous memory.
         * This is not the responsibility of this allocator to free this area.
         * @param area_begin Start of the contiguous memory area
         * @param area_end Past the end address of the memory area */
        linear( void* area_begin, void* area_end ) :
          begin{reinterpret_cast<char*>(area_begin)}, current(begin),
          end{reinterpret_cast<char*>(area_end)}
        {}

        /**@brief Does nothing.
         *
         * A linear allocator cannot free individual allocations. Thus, this
         * function does nothing. You should instead first free all memory
         * by calling go_delete/go_delete_array, then you call reset().*/
        inline void deallocate( void* ) const {}

        /**@brief Reset the allocator.
         *
         * After this call, the whole memory area managed by this allocator
         * can be reused. Be careful to delete the individual allocations by
         * calling go_delete/go_delete_array. */
        inline void reset()
        {
          current = begin;
        }

        /**@brief Get the size of an allocation.
         *
         * This function fetch the size of an object allocated at a specified
         * address.
         * @param allocation Address of the object.
         * @return Size of the object allocated to this address. */
        inline size_t get_allocation_size( void* allocation ) const
        {
          union {
            void* as_void;
            char* as_char;
            uint32_t* as_uint32_t;
          };
          // grab the allocation's size from the first N bytes before the user data
          as_void = allocation;
          as_char -= size_front;
          return *as_uint32_t;
        }

        /**@brief Allocate a new bunch of memory.
         *
         * Allocate a space of memory, with the specified alignment. The allocation
         *
         */
        void* allocate (size_t size, size_t alignment, size_t offset)
        {
          /**
           * Here is the layout we want at the end of this function
           * [...<-offset->| <-size-> ]
           *     ^         ^           ^
           *     |         |           current
           *     |         aligned
           *     user_ptr
           */
          const auto intptr  = reinterpret_cast<uintptr_t>(current + offset);
          const auto aligned = (intptr - 1u + alignment ) & -alignment;
          const auto user_ptr = aligned - offset;
          current = reinterpret_cast<char*>(aligned) + size;

          if( current >= end )
            return nullptr;

          // write the size just before the aligned allocation
          *(reinterpret_cast<uint32_t*>(aligned - size_front )) = uint32_t(size);

          return reinterpret_cast<void*>(user_ptr);
        }
      private:
        char* begin;
        char* current;
        char* end;
      };
    }

    namespace thread_policy {
      struct single_thread {
        inline void enter() const {}
        inline void leave() const {}
      };
    }

    namespace bounds_checking_policy {
      struct no {
        static constexpr size_t size_front = size_t{0};
        static constexpr size_t size_back = size_t{0};
        inline void guard_front( void* ) const {}
        inline void guard_back( void* ) const {}
        inline void check_front( void* ) const {}
        inline void check_back( void* ) const {}
      };

      struct per_allocation {
        static constexpr size_t size_front = size_t{4};
        static constexpr size_t size_back = size_t{4};
        static constexpr uint32_t front_word = uint32_t{0xFEEDFACE};
        static constexpr uint32_t back_word = uint32_t{0x1BADC0DE};
        inline void guard_front( void* ptr ) const
        {
          *(static_cast<uint32_t*>(ptr)) = front_word;
        }
        inline void guard_back( void* ptr ) const
        {
          *(static_cast<uint32_t*>(ptr)) = back_word;
        }
        inline void check_front( void* ptr ) const
        {
          GO_ASSERT(
            *(static_cast<uint32_t*>(ptr)) == front_word,
            "front guard had been overwritten")
            (ptr);
        }
        inline void check_back( void* ptr ) const
        {
          GO_ASSERT(
            *(static_cast<uint32_t*>(ptr)) == back_word,
            "back guard had been overwritten")
            (ptr);
        }
      };
    }

    namespace memory_tracking_policy {
      struct no {
        inline void track( void*, size_t, size_t, const char*, const char*, const char*) const {}
        inline void untrack( void* ) const {}
      };
    }

    namespace memory_tagging_policy {
      struct no {
        inline void tag_allocation( void*, size_t ) const {}
        inline void tag_deallocation( void*, size_t ) const {}
      };
      struct yes {
        static constexpr uint32_t deallocated_word = uint32_t{0xDEADBEEF};
        static constexpr uint32_t allocated_word = uint32_t{0xDEADF00D};
        inline void tag_allocation( void* ptr , size_t size ) const
        {
          char* uint32_end = reinterpret_cast<char*>(ptr) + ((size >> 2) << 2);
          char* end = reinterpret_cast<char*>(ptr) + size;

          while( ptr < uint32_end )
            {
              *static_cast<uint32_t*>(ptr) = allocated_word;
              reinterpret_cast<size_t&>(ptr) += 4;
            }
          while( ptr < end )
            {
              *static_cast<uint8_t*>(ptr) = 0xDD;
            }
        }
        inline void tag_deallocation( void* ptr, size_t size ) const
        {
          char* uint32_end = reinterpret_cast<char*>(ptr) + ((size >> 2) << 2);
          char* end = reinterpret_cast<char*>(ptr) + size;

          while( ptr < uint32_end )
            {
              *static_cast<uint32_t*>(ptr) = deallocated_word;
              reinterpret_cast<size_t&>(ptr) += 4;
            }
          while( ptr < end )
            {
              *static_cast<uint8_t*>(ptr) = 0xFF;
            }
        }
      };
    }
  }
}


# endif 
