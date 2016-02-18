/* Created on: Feb 18, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_
# define GRAPHICS_ORIGIN_TIGHT_BUFFER_MANAGER_H_

# include "../graphics_origin.h"

BEGIN_GO_NAMESPACE namespace tools {

  template< class element, typename handle, uint8_t handle_bits>
  class tight_buffer_manager {
  public:
    tight_buffer_manager( size_t number_of_elements = 0 )
      : m_capacity{ 0 }, m_size{ 0 }, m_next_free_handle_slot{ 0 },
        m_element_buffer{ nullptr }, m_handle_buffer{ nullptr }
    {
      if( number_of_elements )
        grow( number_of_elements );
    }


  private:

    void grow( size_t new_capacity )
    {

    }

    enum { STATUS_FREE = 0, STATUS_ALLOCATED = 1, STATUS_GARBAGE = 2 };

    struct handle_entry {
      handle next_free_index : handle_bits;
      handle counter         : sizeof(handle)*8 - handle_bits - 2;
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

} END_GO_NAMESPACE
# endif 
