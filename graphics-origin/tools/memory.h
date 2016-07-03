# ifndef GRAPHICS_ORIGIN_MEMORY_H_
# define GRAPHICS_ORIGIN_MEMORY_H_

# include "./assert.h"
# include "./log.h"
# include <new>

/**@file
 * @brief Memory management: allocating/deallocating memory.
 *
 * This file groups functionalities related to memory management, to
 * allocate and deallocate memory. The design is adapted from the blog of
 * Stefan Reinalter:
 * https://blog.molecular-matters.com/2011/08/03/memory-system-part-5/
 */
namespace graphics_origin {
  namespace tools {

/**@brief Allocate memory with specific alignment.
 *
 * Make an allocation thanks to a memory arena with a specific alignment.
 * \code{.cpp}
 * vec4* ptr1 = go_new_align( vec4, 4, arena);
 * vec2* ptr2 = go_new_align( vec2, 4, arena){ 1, 2 }; // with constructor
 * \endcode
 */
# define go_new_align(type_,alignement_,arena_) new (arena_.allocate(         \
    sizeof(type_),                                                            \
    alignement_,                                                              \
    __FILE__,                                                                 \
    GO_STRINGIZE(__LINE__),                                                   \
    __PRETTY_FUNCTION__)) type_

/**@brief Allocate memory with standard alignment.
 *
 * Make an allocation thanks to a memory arena. The alignment is automatically
 * deduced from the type of the allocation.
 * \code{.cpp}
 * vec3* ptr = go_new( vec3, arena);
 * \endcode
 */
# define go_new(type_,arena_) go_new_align(type_,alignof(type_),arena_)

/**@brief Delete memory.
 *
 * This macro will delete the memory, that is call the destructor and then free
 * the allocation.
 * \code{.cpp}
 * vec3* ptr = go_new( vec3, arena );
 * go_delete(ptr);
 * \endcode
 */
# define go_delete(object_,arena_) graphics_origin::tools::detail::deallocate( object_, arena_ )

/**@brief Allocate memory for an array with specific alignment.
 *
 * Make an allocation for an array thanks to a memory arena with a specific
 * alignment.
 * \code{.cpp}
 * int* array = go_new_array_align( int[10], 4, arena );
 * \endcode
 */
# define go_new_array_align(type_,alignment_,arena_)                          \
    graphics_origin::tools::detail::new_array< std::remove_all_extents<type_>::type>(\
      arena_,                                                                 \
      std::extent<type_,0>::value,                                            \
      alignment_,                                                             \
      __FILE__,                                                               \
      GO_STRINGIZE(__LINE__),                                                 \
      __PRETTY_FUNCTION__,                                                    \
      std::integral_constant<bool,std::is_pod<type_>::value>() )

/**@brief Allocate memory for an array with standard alignment.
 *
 * Make an allocation for an array thanks to a memory arena. The alignment is
 * automatically deduced from the type of the allocation.
 * \code{.cpp}
 * double* array = go_new_array( double[4], arena );
 * \endcode
 */
# define go_new_array(type_,arena_) go_new_array_align(type_,alignof(type_),arena_)

/**@brief Delete an array.
 *
 * This macro will delete an array, that is calling the destructor of each object
 * of the array, and then free the allocation.
 * \code{.cpp}
 * double* array = go_new_array( double[4], arena );
 * go_delete_array( array );
 * \endcode
 */
# define go_delete_array(object_,arena_) graphics_origin::tools::detail::delete_array( object_, arena_ )

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
          uint32_t* as_uint32_t;
          type* as_type;
        };

        as_void = a.allocate(sizeof(type)*N + sizeof(uint32_t), alignment, file, line, function );

        // store number of instances in first size_t bytes
        *as_uint32_t++ = N;

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
          uint32_t* as_uint32_t;
          type* as_type;
        };

        // user pointer points to first instance...
        as_type = ptr;

        // ...so go back size_t bytes and grab number of instances
        const uint32_t N = as_uint32_t[-1];

        // call instances' destructor in reverse order
        for (uint32_t i=N; i>0; --i)
          as_type[i-1].~T();

        a.deallocate(as_uint32_t-1);
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
          const size_t front_offset = bounds_checking_policy::size_front + allocation_policy::size_front;
          const size_t new_size = size + front_offset + bounds_checking_policy::size_back;
          char* plain_memory = static_cast<char*>( allocator->allocate( new_size, alignment, front_offset ));

          bounds_checker.guard_front( plain_memory );
          memory_tagger.tag_allocation( plain_memory + front_offset, original_size );
          bounds_checker.guard_back( plain_memory + front_offset + original_size );

          memory_tracker.track( plain_memory, new_size, alignment, file, line, function );

        thread_guard.leave();

        return plain_memory + front_offset;
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


    /**@brief Gather different memory area to use in the allocators.
     *
     * A memory area is a memory space that is used to initialize an allocator.
     * The interface is pretty simple:
     * - a begin() function that returns a void* pointer to the beginning of the area
     * - a end() function that returns a void* pointer to past the end of the area
     * - a destructor that will destroy this area, if needed.
     */
    /**@addtogroup memory_area tools::memory_area
     * @{
     */
    namespace memory_area {
      /**@brief Memory area on stack.
       *
       * This class represents a memory area that is located on the stack. As
       * such, it is faster to access it, but it is limited in size (since
       * stack is quite small compared to the heap), and this size must be
       * specified at compile time.
       */
      template< size_t number_of_bytes >
      class on_stack {
      public:
        on_stack() :
          internal_end{ internal + number_of_bytes }
        {}

        ~on_stack(){}

        inline void* begin()
        {
          return internal;
        }

        inline void* end()
        {
          return internal_end;
        }
      private:
        char internal[number_of_bytes];
        void* internal_end;
      };

      /**@brief Memory area on heap.
       *
       * This class represents a memory area that is located on the heap. Its
       * size must be specified at runtime, in the constructor. As such, it can
       * hold much bigger space than the on_stack class.
       */
      class on_heap {
      public:
        on_heap( size_t number_of_bytes ) :
          internal{ new char[number_of_bytes] },
          internal_end{ internal + number_of_bytes }
        {}

        ~on_heap()
        {
          delete[] internal;
          internal = nullptr;
          internal_end = nullptr;
        }

        inline void* begin()
        {
          return internal;
        }

        inline void* end()
        {
          return internal_end;
        }

      private:
        char* internal;
        void* internal_end;
      };

    }
    /**@}*/

    namespace allocation_policy {
      /**@brief Implements a linear allocator.
       *
       * A linear allocator operates on a contiguous memory of fixed size. It
       * cannot free individual allocations. Instead, it
       * frees them all when calling reset. It can be useful to store all
       * the temporary values computed during a frame for example. At the end
       * of each frame, the allocator can be reset.
       *
       * \sa stack, stack_with_lifo_check
       */
      class linear {
      public:
        /**@brief Amount of bytes used in front of the allocation to store information.
         *
         * In front of each allocation, we store only 1 value, stored on 4 bytes, for
         * the size of the allocation.
         */
        static constexpr size_t size_front = sizeof(uint32_t);

        /**@brief Construct a new linear allocator.
         *
         * Instantiate a new linear allocator on an area of contiguous memory.
         * This is not the responsibility of this allocator to free this area.
         * @param area A memory area (such as tools::memory_area::on_stack)*/
        template< class memory_area >
        linear( memory_area& area ) :
          begin{reinterpret_cast<char*>(area.begin())}, current(begin),
          end{reinterpret_cast<char*>(area.end())}
        {}

        /**@brief Does nothing.
         *
         * A linear allocator cannot free individual allocations. Thus, this
         * function does nothing. You should instead first free all memory
         * by calling go_delete() or go_delete_array(), then you call reset().*/
        inline void deallocate( void* ) const {}

        /**@brief Reset the allocator.
         *
         * After this call, the whole memory area managed by this allocator
         * can be reused. Be careful to delete the individual allocations by
         * calling go_delete() or go_delete_array(). */
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
          return reinterpret_cast<uint32_t*>(allocation)[-1];
        }

        /**@brief Allocate a new bunch of memory.
         *
         * Allocate a space of memory, with the specified alignment. The allocation
         *
         */
        void* allocate (size_t size, size_t alignment, size_t offset)
        {
          /*
           * Here is the layout we want at the end of this function
           *     <-------size-------->
           * [...<-offset->|          ]
           *     ^         ^           ^
           *     |         |           current
           *     |         aligned
           *     user_ptr
           */
          const auto intptr  = reinterpret_cast<uintptr_t>(current + offset);
          const auto aligned = (intptr - 1u + alignment ) & -alignment;
          const auto user_ptr = aligned - offset;
          current = reinterpret_cast<char*>(user_ptr) + size;

          if( current > end )
            return nullptr;

          // write the size just before the aligned allocation
          reinterpret_cast<uint32_t*>(aligned)[-1] = uint32_t(size);

          return reinterpret_cast<void*>(user_ptr);
        }
      private:
        char* begin;
        char* current;
        char* end;
      };

      /**@brief Implements a stack allocator.
       *
       * A stack allocator is a LIFO structure, meaning that the last allocation
       * should be freed first. This class does not check you free the allocations
       * in the right order, but stack_with_lifo_check does.
       *
       * \sa linear, stack_with_lifo_check
       */
      class stack {
      public:
        /**@brief Amount of bytes used in front of the allocation to store information.
         *
         * In front of each allocation, we store 2 values:
         * - the size of the allocation
         * - the value offset such that current = start + offset before allocating
         */
        static constexpr size_t size_front = sizeof(uint32_t) + sizeof(uint32_t);

        /**@brief Construct a new stack allocator.
         *
         * Instantiate a new stack allocator on an area of contiguous memory.
         * This is not the responsibility of this allocator to free this area.
         * @param area A memory area (such as tools::memory_area::on_stack)*/
        template< class memory_area >
        stack( memory_area& area ) :
          start{reinterpret_cast<char*>(area.begin())}, current(start),
          end{reinterpret_cast<char*>(area.end())}
        {}
        /**@brief Get the size of an allocation.
         *
         * Fetch the size of an allocation, stored 8 bytes before the allocation.
         * @param allocation A pointer to an allocation returned by allocate()
         * @return The size of the allocation.
         */
        inline size_t get_allocation_size( void* allocation ) const
        {
          return reinterpret_cast<uint32_t*>(allocation)[-2];
        }

        /**@brief Allocate some memory.
         *
         * Make an allocation of the specified size, with the requested alignment and
         * with enough place to store some bytes in front of the allocation.
         * @param size Size of the allocation to make.
         * @param alignment Alignment requested for the allocation.
         * @param offset Number of bytes to reserve in front of the allocation. This
         * number should be larger than <tt>size_front<\tt>.
         * @param A pointer to the allocated memory.
         */
        void* allocate( size_t size, size_t alignment, size_t offset )
        {
          /*
           * Here is the layout we want at the end of this function
           *     <-------size-------->
           * [...<-offset->|          ]
           *     ^         ^           ^
           *     |         |           current
           *     |         aligned
           *     user_ptr
           */
          // The offset to the beginning of the memory area. It will be stored
          // in front of the user pointer, to be restored at deallocation.
          const uint32_t allocation_offset = static_cast<uint32_t>(current - start);
          const auto intptr  = reinterpret_cast<uintptr_t>(current + offset);
          const auto aligned = (intptr - 1u + alignment ) & -alignment;
          const auto user_ptr = aligned - offset;
          current = reinterpret_cast<char*>(user_ptr) + size;

          if( current > end )
            return nullptr;

          reinterpret_cast<uint32_t*>(aligned)[-1] = allocation_offset;
          reinterpret_cast<uint32_t*>(aligned)[-2] = uint32_t(size);

          return reinterpret_cast<void*>(user_ptr);
        }

        /**@brief Free an allocation.
         *
         * Free an allocation made by allocate(). Note that since this is a stack
         * allocator, you should free the memory in reverse order of the
         * allocations. This class does not check you free the allocations in the
         * right order. If this is something you want to be done, see stack_with_lifo_check.
         * @param allocation The allocation to free.
         */
        void deallocate( void* allocation )
        {
          current = start + reinterpret_cast<uint32_t*>(allocation)[-1];
        }

      private:
        char* start;
        char* current;
        char* end;
      };

      /**@brief Implements a stack allocator.
       *
       * A stack allocator is a LIFO structure, meaning that the last allocation
       * should be freed first. Unlike stack, this class checks you free the allocations
       * in the right order.
       *
       * \sa linear, stack
       */
      class stack_with_lifo_check {
      public:
        /**@brief Amount of bytes used in front of the allocation to store information.
         *
         * In front of each allocation, we store 3 values:
         * - the size of the allocation
         * - the value offset such that current = start + offset before allocating
         * - the id of the allocation, increased at each allocation.
         */
        static constexpr size_t size_front = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);

        /**@brief Construct a new stack allocator.
         *
         * Instantiate a new stack allocator on an area of contiguous memory.
         * This is not the responsibility of this allocator to free this area.
         * @param area A memory area (such as tools::memory_area::on_stack)*/
        template< class memory_area >
        stack_with_lifo_check( memory_area& area ) :
          start{reinterpret_cast<char*>(area.begin())}, current(start),
          end{reinterpret_cast<char*>(area.end())}, id{0}
        {}

        /**@brief Get the size of an allocation.
         *
         * Fetch the size of an allocation, stored 12 bytes before the allocation.
         * @param allocation A pointer to an allocation returned by allocate()
         * @return The size of the allocation.
         */
        inline size_t get_allocation_size( void* allocation ) const
        {
          return reinterpret_cast<uint32_t*>(allocation)[-3];
        }

        /**@brief Allocate some memory.
         *
         * Make an allocation of the specified size, with the requested alignment and
         * with enough place to store some bytes in front of the allocation.
         * @param size Size of the allocation to make.
         * @param alignment Alignment requested for the allocation.
         * @param offset Number of bytes to reserve in front of the allocation. This
         * number should be larger than <tt>size_front<\tt>.
         * @param A pointer to the allocated memory.
         */
        void* allocate( size_t size, size_t alignment, size_t offset )
        {
          /*
           * Here is the layout we want at the end of this function
           *     <-------size-------->
           * [...<-offset->|          ]
           *     ^         ^           ^
           *     |         |           current
           *     |         aligned
           *     user_ptr
           */
          // The offset to the beginning of the memory area. It will be stored
          // in front of the user pointer, to be restored at deallocation.
          const uint32_t allocation_offset = static_cast<uint32_t>(current - start);

          const uintptr_t intptr = reinterpret_cast<uintptr_t>(current + offset );
          const auto aligned = (intptr - 1u + alignment ) & -alignment;
          void* user_ptr = reinterpret_cast<void*>( aligned - offset );
          current = reinterpret_cast<char*>(user_ptr) + size;

          if( current > end )
            return nullptr;

          uint32_t* data = reinterpret_cast<uint32_t*>(aligned);
          ++id;
          data[-1] = id;
          data[-2] = allocation_offset;
          data[-3] = uint32_t(size);

          return user_ptr;
        }

        /**@brief Free an allocation.
         *
         * Free an allocation made by allocate(). Note that since this is a stack
         * allocator, you should free the memory in reverse order of the
         * allocations. Otherwise, an assertion will fail.
         * @param allocation The allocation to free.
         */
        void deallocate( void* allocation )
        {
          uint32_t* data = reinterpret_cast<uint32_t*>(allocation);
          GO_ASSERT( data[-1] == id, "you forgot to free some memory before this one")(data,data[-1],id);
          current = start + data[-2];
          --id;
        }

      private:
        char* start;
        char* current;
        char* end;
        uint32_t id;
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
        inline void check_front( const void* ) const {}
        inline void check_back( const void* ) const {}
      };

      struct per_allocation {
        static constexpr uint32_t front_word = uint32_t{0xFEEDFACE};
        static constexpr uint32_t back_word = uint32_t{0x1BADC0DE};
        static constexpr size_t size_front = sizeof(front_word);
        static constexpr size_t size_back = sizeof(back_word);
        inline void guard_front( void* ptr ) const
        {
          *(static_cast<uint32_t*>(ptr)) = front_word;
        }
        inline void guard_back( void* ptr ) const
        {
          *(static_cast<uint32_t*>(ptr)) = back_word;
        }
        inline void check_front( const void* ptr ) const
        {
          uint32_t word = *reinterpret_cast<const uint32_t*>( ptr );
          GO_ASSERT(
            word == front_word,
            "front guard had been overwritten (is not 0xFEEDFACE)")
            (word);
        }
        inline void check_back( const void* ptr ) const
        {
          uint32_t word = *reinterpret_cast<const uint32_t*>( ptr );
          GO_ASSERT(
            word == back_word,
            "back guard had been overwritten (is not 0x1BADC0DE)")
            (word);
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
