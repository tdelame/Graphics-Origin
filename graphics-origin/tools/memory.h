# ifndef GRAPHICS_ORIGIN_MEMORY_H_
# define GRAPHICS_ORIGIN_MEMORY_H_

# include "./assert.h"
# include "./log.h"
/**
 * The design comes from the Molecule Engine, explained by Stefan Reinalter
 * on his blog:
 * https://blog.molecular-matters.com/2011/08/03/memory-system-part-5/
 */
namespace graphics_origin {
  namespace tools {

    template<
      class allocation_policy,
      class thread_policy,
      class bounds_checking_policy,
      class memory_tracking_policy,
      class memory_tagging_policy >
    class memory_arena {
    public:
      template< class memory_area_policy >
      explicit memory_arena( const memory_area_policy& area )
        : allocator( area.get_start(), area.get_end() )
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
          char* plain_memory = static_cast<char*>( allocator.allocate( new_size, alignment, bounds_checking_policy::size_front));

          bounds_checker.guard_front( plain_memory );
          memory_tagger.tag_allocation( plain_memory + bounds_checking_policy::size_front, original_size );
          bounds_checker.guar_back( plain_memory + bounds_checking_policy::size_front + original_size );

          memory_tracker.track( plain_memory, new_size, alignment, file, line, function );

        thread_guard.leave();

        return plain_memory + bounds_checking_policy::size_front;
      }

      void deallocate( void* ptr )
      {
        thread_guard.enter();

          char* original_memory = static_cast<char*>( ptr ) - bounds_checking_policy::size_front;
          const size_t original_size = allocator.get_allocation_size( original_memory );

          bounds_checker.check_front( original_memory );
          bounds_checker.check_back( original_memory + original_size - bounds_checking_policy::size_back );
          memory_tracker.untrack( original_memory );
          memory_tagger.tag_deallocation( original_memory, original_size );

          allocator.deallocate( original_memory );
        thread_guard.leave();
      }

    private:
      allocation_policy allocator;
      thread_policy thread_guard;
      bounds_checking_policy bounds_checker;
      memory_tracking_policy memory_tracker;
      memory_tagging_policy memory_tagger;
    };

    struct single_thread_policy {
      inline void enter() const {}
      inline void leave() const {}
    };

    struct no_bounds_checking_policy {
      static constexpr size_t size_front = size_t{0};
      static constexpr size_t size_back = size_t{0};
      inline void guard_front( void* ) const {}
      inline void guard_back( void* ) const {}
      inline void check_front( void* ) const {}
      inline void check_back( void* ) const {}
    };

    struct simple_bounds_checking_policy {
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
          *(static_cast<uint32_t*>(ptr) == front_word ),
          "front guard had been overwritten")
          (ptr);
      }
      inline void check_back( void* ptr ) const
      {
        GO_ASSERT(
          *(static_cast<uint32_t*>(ptr) == back_word ),
          "back guard had been overwritten")
          (ptr);
      }
    };

    struct no_memory_tracking_policy {
      inline void track( void*, size_t, size_t, const char*, const char*, const char*) const {}
      inline void untrack( void* ) const {}
    };

    struct no_memory_tagging_policy {
      inline void tag_allocation( void*, size_t ) const {}
      inline void tag_deallocation( void*, size_t ) const {} //DEADBEEF
    };

  }
}


# endif 
