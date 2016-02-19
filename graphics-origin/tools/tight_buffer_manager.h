/* Created on: Feb 18, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_
# define GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_

# include "../graphics_origin.h"

# include <type_traits>

BEGIN_GO_NAMESPACE namespace tools {

  /**@brief Cannot have more elements in a tight buffer manager.
   *
   * This exception is thrown when the maximal capacity of a tight buffer
   * manager is reached during the creation of an element. This maximal
   * capacity is fixed by the number of bits used to store the index of
   * an element in a handle.
   */
  struct tight_buffer_manager_buffer_overflow
      : public std::runtime_error
  {
    tight_buffer_manager_buffer_overflow( const std::string& file, size_t line )
        : std::runtime_error( "cannot add more elements to buffer at line "
            + std::to_string( line ) + " of file " + file )
    {}
  };

  /**@brief An invalid handle was passed to a tight buffer manager.
   *
   * Such an exception is thrown when:
   * - the index of an handle is out of bounds,
   * - the counter of the handle is different from the one in the handle buffer
   * (meaning the element pointed by the handle has already been deleted and another
   * element is pointed by an handle with the same index has been created)
   * - there is no allocated memory pointed by such handle (meaning the element
   * pointed by the handle has been deleted)
   */
  struct tight_buffer_manager_invalid_handle
      : public std::runtime_error
  {
    tight_buffer_manager_invalid_handle( const std::string& file, size_t line )
        : std::runtime_error( "invalid handle at line "
            + std::to_string( line ) + " of file " + file )
    {}
  };

  /**@brief An invalid element pointer was passed to a tight buffer manager.
   *
   * Such an exception occurs when the memory pointed by a pointer is outside
   * of the element buffer or if it is not correctly aligned inside the memory
   * of the element buffer.
   */
  struct tight_buffer_manager_invalid_element_pointer
      : public std::runtime_error
  {
    tight_buffer_manager_invalid_element_pointer( const std::string& file, size_t line )
        : std::runtime_error( "the given pointer is not managed by the tight buffer at line "
            + std::to_string( line ) + " of file " + file )
    {}
  };

  /**@brief An invalid element index was passed to a tight buffer manager.
   *
   * Such an exception occurs when an index i does not correspond to a valid element
   * element_buffer[i] of the element buffer.
   */
  struct tight_buffer_manager_invalid_element_index
      : public std::runtime_error
  {
    tight_buffer_manager_invalid_element_index( const std::string& file, size_t line )
        : std::runtime_error( "invalid element index at line "
            + std::to_string( line ) + " of file " + file )
    {}
  };

  /**@brief Tight buffer managed by handles.
   *
   * This class stores a set of elements contiguously in an element buffer. The
   * difference with a vector is that elements are managed by handles. Those
   * handles are guaranteed to remain the same as long as the elements designated
   * by such handles do not change (event if a reallocation of the element buffer
   * occurs and if elements are moved after an element is deleted).
   *
   * The \a element template parameter designate what you want to store inside
   * a tight buffer. It should be default constructible and move assignable.
   *
   * The \a handle_type specifies the integral type that will be used to store
   * the index and the counter of an handle. Thus, it must integral and unsigned.
   */
  template< class element, typename handle_type, uint8_t index_bits>
  class tight_buffer_manager {
    static_assert(
        std::is_default_constructible< element >::value,
        "element type is not default constructible");
    static_assert(
        std::is_move_assignable< element >::value,
        "element type is not move assignable");
    static_assert(
        std::is_integral< handle_type >::value,
        "handle type is not an integral type");
    static_assert(
        std::is_unsigned< handle_type >::value,
        "handle type is not unsigned");
    static_assert(
       index_bits + 2 < sizeof(handle_type) * 8,
       "handle type is not large enough to have the required bits for the index");
    static_assert(
        index_bits > 0,
        "you should have at least one bit to represent an index, otherwise you cannot store any element");

    static constexpr uint8_t handle_bits;
    static constexpr size_t max_index;
    static constexpr size_t max_counter;

  public:
    /**@brief An handle to designate an element.
     *
     * The handle is composed of two fields:
     * - index, that gives the index of the element in the tight
     * element buffer
     * - counter, that tells the 'version' of the element at that index
     * in the tight buffer (how many time an element had been allocated
     * at this index).
     */
    struct handle {
      handle_type index  : index_bits;
      handle_type counter: handle_bits - index_bits;

      handle()
        : index{ max_index },
          counter{ max_counter + 1 }
      {}

      handle( handle_type i, handle_type c )
        : index{ i }, counter{ c }
      {}

      inline operator handle_type() const
      {
        return (counter << index_bits) | index;
      }
      bool is_valid() const noexcept
      {
        return counter <= max_counter;
      }
    };

    tight_buffer_manager( size_t number_of_elements = 0 )
      : m_capacity{ 0 }, m_size{ 0 }, m_next_free_handle_slot{ 0 },
        m_element_buffer{ nullptr }, m_element_to_handle{ nullptr },
        m_handle_buffer{ nullptr }
    {
      if( number_of_elements )
        grow( number_of_elements );
    }

    ~tight_buffer_manager()
    {
      delete[] m_element_buffer;
      delete[] m_element_to_handle;
      delete[] m_handle_buffer;
    }

    size_t get_size() const noexcept
    {
      return m_size;
    }

    size_t get_capacity() const noexcept
    {
      return m_capacity;
    }

    size_t get_max_capacity() const noexcept
    {
      return max_index;
    }

    std::pair<handle, element&> create()
    {
      if( m_size == m_capacity )
        {
          grow( std::min( max_index, m_capacity + std::max( m_capacity, size_t{10} ) ) );
        }
      const auto handle_index = m_next_free_handle_slot;
      auto entry = m_handle_buffer + handle_index;

      // update the entry
      ++entry->counter;
      if( entry->counter > max_counter )
        entry->counter = 0;
      entry->index = m_size;
      entry->next_free_index = 0;
      entry->status = STATUS_ALLOCATED;

      // map the element to the handle entry
      m_element_to_handle[ entry->index ] = handle_index;

      std::pair<handle, element&> result = std::make_pair(
          handle( handle_index, entry->counter ),
          m_element_buffer[m_size] );

      // update this
      m_next_free_handle_slot = entry->next_free_index;
      ++m_size;

      return result;
    }

    void remove( handle h )
    {
      if ( h.index >= m_capacity )
        throw tight_buffer_manager_invalid_handle( __FILE__, __LINE__ );
      auto entry = m_handle_buffer + h.index;
      if( entry->status != STATUS_ALLOCATED || entry->counter != h.counter )
        throw tight_buffer_manager_invalid_handle( __FILE__, __LINE__ );

      entry->next_free_index = m_next_free_handle_slot;
      entry->status = STATUS_FREE;

      m_next_free_handle_slot = h.index;
      if( --m_size && entry->index != m_size )
        {
          m_element_buffer[ entry->index ] = std::move( m_element_buffer[ m_size ] );
          m_handle_buffer[ m_element_to_handle[ entry->index ] ].index = entry->index;
        }
    }

    void remove( element* e )
    {
      if( e < m_element_buffer || e >= m_element_buffer + m_size )
        throw tight_buffer_manager_invalid_element_pointer( __FILE__, __LINE__ );

      const auto element_index = std::distance( m_element_buffer, e );
      if( m_element_buffer + element_index != e )
        throw tight_buffer_manager_invalid_element_pointer( __FILE__, __LINE__ );

      const auto entry_index = m_element_to_handle[ element_index ];

      auto entry = m_handle_buffer + entry_index;

      entry->next_free_index = m_next_free_handle_slot;
      entry->status = STATUS_FREE;

      m_next_free_handle_slot = entry_index;
      if( --m_size && entry->index != m_size )
        {
          m_element_buffer[ entry->index ] = std::move( m_element_buffer[ m_size ] );
          m_handle_buffer[ m_element_to_handle[ entry->element_index ] ].element_index = entry->element_index;
        }
    }

    element& get( handle h )
    {
      if( h.index >= m_capacity )
        throw tight_buffer_manager_invalid_handle( __FILE__, __LINE__ );
      const auto entry = m_handle_buffer + h.index;
      if( entry->status != STATUS_ALLOCATED || entry->counter != h.counter )
        throw tight_buffer_manager_invalid_handle( __FILE__, __LINE__ );
      return m_element_buffer[ entry->index ];
    }

    element& get_by_index( size_t index )
    {
      if( index >= m_size )
        throw tight_buffer_manager_invalid_element_index( __FILE__, __LINE__ );
      return m_element_buffer[ index ];
    }

    handle get_handle( size_t index )
    {
      if( index >= m_size )
        throw tight_buffer_manager_invalid_element_index( __FILE__, __LINE__ );
      auto handle_index = m_element_to_handle[ index ];
      return handle( handle_index, m_handle_buffer[ handle_index ].counter );
    }

  private:
    void grow( size_t new_capacity )
    {
      if( new_capacity <= m_capacity || new_capacity > max_index )
        throw tight_buffer_manager_buffer_overflow( __FILE__, __LINE__ );
      if( !m_element_buffer )
        {
          m_element_buffer = new element[ new_capacity ];
          m_element_to_handle = new size_t[ new_capacity ];
          m_handle_buffer = new handle_entry[ new_capacity ];
        }
      else
        {
          auto new_element_buffer = new element[ new_capacity ];
          auto new_element_to_handle = new size_t[ new_capacity ];
          auto new_handle_buffer = new handle_entry[ new_capacity ];
          # pragma omp parallel for
          for( size_t i = 0; i < m_capacity; ++ i )
            {
              new_element_buffer[ i ] = std::move( m_element_buffer[ i ] );
              new_element_to_handle[ i ] = m_element_to_handle[ i ];
              new_handle_buffer[ i ] = m_handle_buffer[ i ];
            }
          delete[] m_element_buffer;
          delete[] m_element_to_handle;
          delete[] m_handle_buffer;

          m_element_buffer = new_element_buffer;
          m_element_to_handle = new_element_to_handle;
          m_handle_buffer = new_handle_buffer;
        }
      # pragma omp parallel for
      for( size_t i = m_capacity; i < new_capacity; ++ i )
        m_handle_buffer[ i ].next_free_index = i + 1;
      m_capacity = new_capacity;
    }

    enum { STATUS_FREE = 0, STATUS_ALLOCATED = 1, STATUS_GARBAGE = 2 };

    struct handle_entry {
      handle_type next_free_index : index_bits;
      handle_type counter         : handle_bits - index_bits - 2;
      handle_type status          : 2;

      size_t element_index;

      handle_entry()
        : next_free_index{ 0 }, counter{ 0 },
          status{ STATUS_FREE }, element_index{ 0 }
      {}
    };

    size_t m_capacity;
    size_t m_size;
    size_t m_next_free_handle_slot;

    element* m_element_buffer;
    size_t* m_element_to_handle;
    handle_entry* m_handle_buffer;

  };

  template< class element, typename handle_type, uint8_t index_bits >
  constexpr uint8_t tight_buffer_manager< element, handle_type, index_bits>::handle_bits = sizeof(handle_type) << 3;

  template< class element, typename handle_type, uint8_t index_bits >
  constexpr size_t tight_buffer_manager< element, handle_type, index_bits>::max_index = (1 << index_bits) - 1;

  template< class element, typename handle_type, uint8_t index_bits >
  constexpr size_t tight_buffer_manager< element, handle_type, index_bits>::max_counter =
      (1 << (tight_buffer_manager<element, handle_type, index_bits>::handle_bits - index_bits - 2)) - 1;

} END_GO_NAMESPACE
# endif 
