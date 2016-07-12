# ifndef GRAPHICS_ORIGIN_MEMORY_H_
# define GRAPHICS_ORIGIN_MEMORY_H_

# include "./assert.h"
# include "./log.h"
# include <new>

// virtual_memory namespace
# ifdef _WIN32
#  include "WinBase.h"
# elif __linux__
#  include <sys/mman.h>
#  include <unistd.h>
# else
#  error "unknown platform"
# endif

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

      inline void* align( void* pointer, size_t alignment )
      {
        return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>( pointer ) - 1u + alignment ) & -alignment);
      }
    }


    /**@brief Managing virtual memory.
     *
     * Virtual memory is a nice tool that can be used to build efficient
     * applications. This namespace gather some functions to ease the use
     * of virtual memory.
     */
    /**@addtogroup memory_area tools::virtual_memory
     * @{
     */
    namespace virtual_memory {

      inline size_t get_page_size()
      {
# ifdef _WIN32
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwPageSize
# elif __linux__
        return sysconf(_SC_PAGESIZE);
# endif
      }

      /**
       *
       *
       */
      inline void* allocate_address_space( size_t max_size_in_bytes )
      {
# ifdef _WIN32
        return VirtualAlloc( nullptr, max_size_in_bytes, MEM_RESERVE, PAGE_NOACCESS );
# elif __linux__
        void* result = mmap( nullptr, max_size_in_bytes, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0 );
        msync( result, max_size_in_bytes, MS_SYNC | MS_INVALIDATE );
        return result;
# endif
      }

      /**
       * Allocate physical memory and map it into the virtual address space.
       * This operation is also known as committing the memory, hence the name
       * of the function.
       */
      inline void commit_memory( void* start, size_t size )
      {
# ifdef _WIN32
        VirtualAlloc( start, size, MEM_COMMIT, PAGE_READWRITE );
# elif __linux__
//        mprotect( start, size, PROT_READ | PROT_READ );

        mmap( start, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANON, -1, 0 );
        msync( start, size, MS_SYNC | MS_INVALIDATE );
//        void * ptr = mmap(addr, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED|MAP_ANON, -1, 0);
//         msync(addr, size, MS_SYNC|MS_INVALIDATE);
//         return ptr;
# endif
      }

      /**
       * Un-map the physical memory and return it to the operating system.
       */
      inline void decommit_memory( void* start, size_t size_in_bytes )
      {
# ifdef _WIN32
        VirtualFree( start, size_in_bytes, MEM_DECOMMIT );
# elif __linux__
        mmap(start, size_in_bytes, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0);
        msync(start, size_in_bytes, MS_SYNC|MS_INVALIDATE);
# endif
      }

      inline void free_address_space( void* start, size_t size_in_bytes )
      {
# ifdef _WIN32
        VirtualFree( start, 0, MEM_RELEASE );
# elif __linux__
        auto result = munmap( start, size_in_bytes );
        GO_ASSERT( result >= 0, "freeing physical memory failed" )(result,start,size_in_bytes);
# endif
      }
    }

    /**
     *
     * This class is responsible for assembling all the elements of a memory
     * management system:
     * - the allocation policy, that performs the allocation and deallocation of
     * the memory
     * - the thread policy, to configure the system to be used in a single
     * thread or in multiple threads
     * - the bounds checking policy, used to detect an overflow, that is when
     * something is written outside of the memory space handed to the user
     * - the memory tracking policy, to store information about where
     * and when an allocation is made, e.g. to correct memory leaks
     * - the memory tagging policy, to tag the memory handed to the user
     * at the allocation and deallocation, to check for uninitialized
     * memory as well as using memory already freed.
     *
     * When an allocation of size s is requested by the user, internally
     * the allocation will allocate the following memory space:
     * \code{.cpp}
     * [ allocation data | bounds checker front | user data | bounds checker back ]
     *                                           <--- s --->
     * \endcode
     *
     * Some implementations for those elements are already given. However,
     * you can make your own implementation. Next are the required for each
     * of the elements.
     *
     * For the allocation policy, the implementation is required to have:
     * - a public constant value named \c size_front that contains the size
     * in bytes of the allocation data stored in front of the memory handed
     * to the user
     * - a function <tt> void allocate( size_t size, size_t alignment, size_t offset) </tt>
     * to perform an allocation of the requested size, with the user memory aligned with
     * the requested alignment. The user memory starts at the specified offset, that is
     * after any data stored there by the allocation policy and the bounds checking policy.
     *
     * The thread policy will add critical section to make sure the memory system
     * can be used in different thread. To do so, an implementation must provide
     * the following functions:
     * - <tt> void enter() </tt> to enter in a critical section
     * - <tt> void leave() </tt> to leave a critical section.
     *
     * The bounds checking policy must provide the following elements:
     * - public constant values names \c size_front and \c size_back containing
     * respectively the size of the data stored in front and at the back of the
     * user memory
     * - a function <tt> void guard_front( void* front_memory ) </tt> to write the
     * front data (of size \c size_front)
     * - a function <tt> void guard_back( void* back_memory ) </tt> to write the
     * back data (of size \c size_back)
     * - a function <tt> void check_front( void* front_memory ) </tt> to check if
     * the front data is still the same
     * - a function <tt> void check_back( void* back_memory ) </tt> to check if
     * the back data is still the same.
     *
     * The memory tracking policy must provide the two following functions:
     * - <tt> void track( void* allocated_memory, size_t allocation_size, size_t alignment,
     *  const char* filename, const char* line_number, const char* function_name) </tt>
     *  to track the origin of an allocation
     * - <tt> void untrack( void* allocated_memory ) </tt> to forget about an allocation,
     * after the memory had been deallocated.
     *
     * The memory tagging policy with the two following functions:
     * - <tt> void tag_allocation( void* user_memory, size_t size ) </tt>
     * to tag the memory part that will be handed to the user after an allocation.
     * - <tt> void tag_dallocation( void* user_memory, size_t size ) </tt>
     * to tag the memory part (that was handed to the user) after a deallocation.
     */
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
          size_t size_in_bytes,
          size_t alignment,
          const char* file,
          const char* line,
          const char* function )
      {
        thread_guard.enter();
          const size_t allocation_size = size_in_bytes + front_offset + bounds_checking_policy::size_back;
          char* plain_memory = static_cast<char*>( allocator->allocate( allocation_size, alignment, front_offset ));
          char* user_memory = plain_memory + front_offset;

          bounds_checker.guard_front( plain_memory + allocation_policy::size_front );
          memory_tagger.tag_allocation( user_memory, size_in_bytes );
          bounds_checker.guard_back( user_memory + size_in_bytes );

          memory_tracker.track( plain_memory, allocation_size, alignment, file, line, function );

        thread_guard.leave();

        return user_memory;
      }

      void deallocate( void* user_memory )
      {
        thread_guard.enter();

          char* plain_memory = static_cast<char*>( user_memory ) - front_offset;
          const size_t allocation_size = allocator->get_allocation_size( plain_memory );
          const size_t size_in_bytes = allocation_size - front_offset - bounds_checking_policy::size_back;

          bounds_checker.check_front( plain_memory + allocation_policy::size_front );
          bounds_checker.check_back( reinterpret_cast<char*>(user_memory) + size_in_bytes );
          memory_tracker.untrack( plain_memory );
          memory_tagger.tag_deallocation( user_memory, size_in_bytes );

          allocator->deallocate( plain_memory );
        thread_guard.leave();
      }

    private:
      static constexpr size_t front_offset = bounds_checking_policy::size_front + allocation_policy::size_front;
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
          return reinterpret_cast<uint32_t*>(allocation)[0];
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
          char* aligned = (char*)detail::align( current + offset, alignment );
          char* user_ptr = aligned - offset;
          current = user_ptr + size;

          if( current > end )
            return nullptr;

          // write the size just before the aligned allocation
          reinterpret_cast<uint32_t*>(user_ptr)[0] = uint32_t(size);

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
         * Fetch the size of an allocation, stored at the first 4 bytes of the allocation.
         * @param allocation A pointer to an allocation returned by allocate()
         * @return The size of the allocation.
         */
        inline size_t get_allocation_size( void* allocation ) const
        {
          return reinterpret_cast<uint32_t*>(allocation)[0];
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
          char* aligned = (char*)detail::align( current + offset, alignment );
          char* user_ptr = aligned - offset;
          current = user_ptr + size;

          if( current > end )
            return nullptr;

          reinterpret_cast<uint32_t*>(user_ptr)[0] = uint32_t(size);
          reinterpret_cast<uint32_t*>(user_ptr)[1] = allocation_offset;

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
          current = start + reinterpret_cast<uint32_t*>(allocation)[1];
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
         * Fetch the size of an allocation, stored at the first 4 bytes of the allocation.
         * @param allocation A pointer to an allocation returned by allocate()
         * @return The size of the allocation.
         */
        inline size_t get_allocation_size( void* allocation ) const
        {
          return reinterpret_cast<uint32_t*>(allocation)[0];
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
          char* aligned = (char*)detail::align( current + offset, alignment );
          char* user_ptr = aligned - offset;
          current = user_ptr + size;

          if( current > end )
            return nullptr;

          ++id;
          reinterpret_cast<uint32_t*>(user_ptr)[0] = uint32_t(size);
          reinterpret_cast<uint32_t*>(user_ptr)[1] = id;
          reinterpret_cast<uint32_t*>(user_ptr)[2] = allocation_offset;

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
          GO_ASSERT( data[2] == id, "you forgot to free some memory before this one")(data,data[-1],id);
          current = start + data[1];
          --id;
        }

      private:
        char* start;
        char* current;
        char* end;
        uint32_t id;
      };


      class growing_stack {
      public:

        static constexpr size_t size_front = sizeof(uint32_t) + sizeof(uint32_t);

        growing_stack( uint32_t max_size_in_bytes, uint32_t grow_size_in_bytes )
          : virtual_start( (char*)virtual_memory::allocate_address_space( max_size_in_bytes ) )
          , virtual_end( virtual_start + max_size_in_bytes )
          , physical_current( virtual_start )
          , physical_end( virtual_start )
          , max_size( max_size_in_bytes )
          , grow_size( grow_size_in_bytes )
        {}

        ~growing_stack()
        {
          virtual_memory::free_address_space( virtual_start, max_size );
        }

        inline size_t get_allocation_size( void* allocation ) const
        {
          return reinterpret_cast<uint32_t*>(allocation)[0];
        }

        void* allocate( size_t size, size_t alignment, size_t offset)
        {
          /*
           * Here is the layout we want at the end of this function
           *     <-------size-------->
           * [...<-offset->|          ]
           *     ^         ^           ^
           *     |         |           physical_current
           *     |         aligned
           *     user_ptr
           */
          // The offset to the beginning of the memory area. It will be stored
          // in front of the user pointer, to be restored at deallocation.
          const uint32_t allocation_offset = static_cast<uint32_t>(physical_current - virtual_start );
          char* aligned = (char*)detail::align( physical_current + offset, alignment );
          char* user_ptr = aligned - offset;
          physical_current = user_ptr + size;

          // not enough physical memory left
          if( physical_current > physical_end )
            {
              // check if we can still get physical pages from the remaining virtual memory
              const size_t needed_physical_size = (( size + grow_size - 1 ) / grow_size ) * grow_size;
              if( physical_end + needed_physical_size > virtual_end )
                return nullptr;

              // allocate new memory pages at the end of already allocated pages
              virtual_memory::commit_memory( physical_end, needed_physical_size );
              physical_end += needed_physical_size;
            }

          reinterpret_cast<uint32_t*>(user_ptr)[0] = uint32_t(size);
          reinterpret_cast<uint32_t*>(user_ptr)[1] = allocation_offset;

          return reinterpret_cast<void*>(user_ptr);
        }

        void deallocate( void* allocation )
        {
          physical_current = virtual_start + reinterpret_cast<uint32_t*>(allocation)[1];
        }

        void purge()
        {
          char* address_to_free = (char*)detail::align( physical_current, grow_size );
          virtual_memory::decommit_memory( address_to_free, physical_end - address_to_free );
          physical_end = address_to_free;
        }

        size_t get_committed_memory() const
        {
          return physical_end - virtual_start;
        }

      private:
        char* virtual_start;
        char* virtual_end;
        char* physical_current;
        char* physical_end;
        const uint32_t max_size;
        const uint32_t grow_size;
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
              ++reinterpret_cast<size_t&>(ptr);
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
              ++reinterpret_cast<size_t&>(ptr);
            }
        }
      };
    }
  }
}


# endif 
