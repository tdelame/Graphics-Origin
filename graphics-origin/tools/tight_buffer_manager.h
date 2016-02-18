/* Created on: Feb 18, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_
# define GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_

# include "../graphics_origin.h"

BEGIN_GO_NAMESPACE namespace tools {

  template< class element, typename handle, uint8_t index_bits>
  class tight_buffer_manager {
    static_assert(
       index_bits + 2 < sizeof(handle) * 8,
       "handle type is not large enough to have the required bits for the index");

  public:
    tight_buffer_manager( size_t number_of_elements = 0 )
      : m_capacity{ 0 }, m_size{ 0 }, m_next_free_handle_slot{ 0 },
        m_element_buffer{ nullptr }, m_handle_buffer{ nullptr }
    {
      if( number_of_elements )
        grow( number_of_elements );
    }

    ~tight_buffer_manager()
    {
      delete[] m_element_buffer;
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

  private:

    static const size_t max_index;

    struct buffer_overflow
        : public std::runtime_error
    {
      buffer_overflow( const std::string& file, size_t line )
          : std::runtime_error( "cannot add more elements to buffer at line "
              + std::to_string( line ) + " of file " + file )
      {}
    };

    void grow( size_t new_capacity )
    {
      assert( new_capacity > m_capacity );
      if( new_capacity >= max_index )
        throw buffer_overflow( __FILE__, __LINE__ );
      if( !m_element_buffer )
        {
          m_element_buffer = new element[ new_capacity ];
          m_handle_buffer = new handle_entry[ new_capacity ];
        }
      else
        {
          auto new_element_buffer = new element[ new_capacity ];
          auto new_handle_buffer = new handle_entry[ new_capacity ];
          # pragma omp parallel for
          for( size_t i = 0; i < m_capacity; ++ i )
            {
              new_element_buffer[ i ] = std::move( m_element_buffer[ i ] );
              new_handle_buffer[ i ] = m_handle_buffer[ i ];
            }
          delete[] m_element_buffer;
          delete[] m_handle_buffer;
          m_element_buffer = new_element_buffer;
          m_handle_buffer = new_handle_buffer;
        }
      # pragma omp parallel for
      for( size_t i = m_capacity; i < new_capacity; ++ i )
        m_handle_buffer[ i ].next_free_index = i + 1;
      m_capacity = new_capacity;
    }

    enum { STATUS_FREE = 0, STATUS_ALLOCATED = 1, STATUS_GARBAGE = 2 };

    struct handle_entry {
      handle next_free_index : index_bits;
      handle counter         : sizeof(handle)*8 - index_bits - 2;
      handle status          : 2;

      size_t index;

      handle_entry()
        : next_free_index{ 0 }, counter{ 0 },
          status{ STATUS_FREE }, index{ 0 }
      {}
    };

    size_t m_capacity;
    size_t m_size;
    size_t m_next_free_handle_slot;

    element* m_element_buffer;
    handle_entry* m_handle_buffer;

  };

  template< class element, typename handle, uint8_t index_bits >
  const size_t tight_buffer_manager< element, handle, index_bits>::max_index = (1 << index_bits) - 1;

} END_GO_NAMESPACE
# endif 
