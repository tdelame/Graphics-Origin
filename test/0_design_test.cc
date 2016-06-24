/**
 * Toward a resource manager.
 *
 * First of all, I believe that resources should be stored/managed by type. I
 * am not sure this is the right way to do so, but I have the impression I
 * should start like that.
 * => one manager by resource type
 *
 * Also, I do not feel I will need to store managers in a container. Thus, there
 * is no reason to make all resources or managers inherit from a common base. We
 * can then use template.
 * => use templates
 *
 * Should we use UID (unique identifiers), handles or pointers to refer to resources?
 * - UID
 *   × by using string hashes, we can have something like get( "building_04" )
 *   × a hash value will be used instead of the string to search into the resource database
 *   × to avoid hashing the string every time, we could do:
 *   constexpr uint32_t building_04_id = hash( "building_04" );
 *   ...
 *   get( building_04_id );
 *     this way, the hash will be computed at compile time. The issue is for
 *     scripts, where the hash should be computed once and then cached.
 *   × collisions should be handled. I have seen a method to do so [1,2], but it may
 *     a little too complex for now. Instead, I propose to build a dictionary, built
 *     when the hash() function is called, and throw an exception when two words are
 *     given the same hash value. It may be brutal, but it works. When a user have
 *     an exception, he/she knows what words are in conflict and can change one of
 *     them.
 * - handles
 *   × nice way to manage resources in a tight buffer
 *   × how to get an handle for a resource? At resource creation, the handle can
 *     then be deployed to where the resource is used. We must then know where the
 *     resource is needed. An alternative, is to have a dictionary to go from a
 *     name to a handle, and add an entry a resource creation with the couple
 *     (name,handle). Still, the resource should be created before the handle is
 *     recovered from the dictionary. A simple fix could be to return a null handle
 *     if a resource with this name is not known. Then, we have a null handle, we
 *     can ask again to the dictionary if it knows the handle of a name. Thus, at
 *     every use of a handle, we should check if it is null. Note that this should
 *     not be done in the tight buffer, since what to return in case of a null
 *     handle? Instead, we can return an empty resource object, that will be filled
 *     when a resource with that name is loaded.
 *   × when a resource is reloaded, it should use the same memory place, to have
 *     the same handle.
 * - pointers
 *   × we could say that with basic pointers, a resource cannot be reloaded. If
 *     the raw memory is encapsulated inside a wrapper, I do not see why not.
 *   × we could say that resources will not be in contiguous memory. Still, if
 *     we wanted to contiguous memory, we would use an array, so I do not see
 *     the issue here.
 *   × How to get the pointers? We would go by name or by handles. Thus, this
 *     case is just an implementation detail of UID and handles.
 *
 * I have seen examples with UID, handles and pointers on the web, but I do not
 * make a clear separation between the three cases. I cannot see why we cannot use
 * the three. So, the main questions are:
 * - how to store the resources, both on client side and inside the manager?
 * - how to handle the destruction of a resource? e.g. when client still has a copy
 *
 * When a resource is destroyed, it is not clear to me that this resource should
 * continue to exist if it is still referred to by other parts of the code. For
 * example, if I destroy a particular mesh, I do not expect to still see it on
 * the screen: instances using that resource should now use an empty mesh. On the
 * other hand, an instance of empty mesh will still exist after the deletion of the
 * resource. Even if the memory consumption is expected to be lower, we still use
 * a mesh resource. Also, a segmentation fault occurring when accessing to a
 * destroyed resource will provide better debugging information.
 *
 * For now, I will simply store the resources inside the map. Clients will have
 * access to those resources thanks to references, meaning they should not keep such
 * references after resource deletion.
 *
 *
 * [1] http://cowboyprogramming.com/2007/01/04/practical-hash-ids/
 *
 * [2] http://www.randygaul.net/2015/12/11/preprocessed-strings-for-asset-ids/
 */

# include "../graphics-origin/graphics_origin.h"
# include "../graphics-origin/tools/log.h"
# include <unordered_map>
namespace graphics_origin {

  uint32_t hash_name( const std::string& lower_case_ascii_name )
  {
    return std::hash<std::string>()( lower_case_ascii_name );
  }

  template< typename resource_type >
  class resource_manager {
  public:

    class resource :
        public resource_type {
    public:
      static resource null;

      bool is_null() const noexcept
      {
        return this == &null;
      }

      uint32_t get_uid() const noexcept
      {
        return uid;
      }

      uint32_t get_scope() const noexcept
      {
        return scope;
      }

      resource( uint32_t id ) :
        uid{id}, scope{0}
      {
        LOG(debug, "a resource [0x" << std::hex <<  uid << "] is created " << this );
      }

      resource() :
        uid{0}, scope{0}
      {
        LOG(debug, "default resource is created " << this );
      }

      resource& operator=( resource&& other )
      {
        scope = other.scope;
        uid = other.uid;
        resource_type::operator=(std::move(other));
        return *this;
      }

    private:
      resource( const resource& other );

      friend resource_manager;
      uint32_t uid;
      uint32_t scope;
    };

    resource_manager()
    {}

    void load(
        const std::string& filename,
        const std::string& lower_case_ascii_name )
    {
      // what should we do if a resource with that name already exist?
      uint32_t id = hash_name( lower_case_ascii_name );
      auto res = resources.emplace( id, id );
      if( !res.second )
        {
          try
            {
              res.first->second.~resource();
              new((void*)&res.first->second) resource( id/*constructor parameters here*/);
              LOG( debug, "resource [0x" << std::hex << id << " re")
            }
          catch(...)
            {
              LOG( warning, "something went wrong in the construction of resource " << filename << ". Using default resource." );
              new((void*)&res.first->second) resource( id );
            }
        }
      //return id?
    }

    resource& get( uint32_t uid )
    {
      auto search = resources.find( uid );
      if( search != resources.end() )
        return search->second;
      return resource::null;
    }

  private:
    std::unordered_map< uint32_t, resource > resources;
  };

  template< typename resource_type >
  typename resource_manager<resource_type>::resource
  resource_manager<resource_type>::resource::null = resource_manager<resource_type>::resource{};


  struct test_data {
    test_data() :
      dummy{ 0 }
    {}
    int dummy;
  };

  namespace cexp
  {

      // Small implementation of std::array, needed until constexpr
      // is added to the function 'reference operator[](size_type)'
      template <typename T, std::size_t N>
      struct array {
          T m_data[N];

          using value_type = T;
          using reference = value_type &;
          using const_reference = const value_type &;
          using size_type = std::size_t;

          // This is NOT constexpr in std::array until C++17
          constexpr reference operator[](size_type i) noexcept {
              return m_data[i];
          }

          constexpr const_reference operator[](size_type i) const noexcept {
              return m_data[i];
          }

          constexpr size_type size() const noexcept {
              return N;
          }
      };

  }

  // Generates CRC-32 table, algorithm based from this link:
  // http://www.hackersdelight.org/hdcodetxt/crc.c.txt
  constexpr auto gen_crc32_table() {
      constexpr auto num_bytes = 256;
      constexpr auto num_iterations = 8;
      constexpr auto polynomial = 0xEDB88320;

      auto crc32_table = cexp::array<uint32_t, num_bytes>{};

      for (auto byte = 0u; byte < num_bytes; ++byte) {
          auto crc = byte;

          for (auto i = 0; i < num_iterations; ++i) {
              auto mask = -(crc & 1);
              crc = (crc >> 1) ^ (polynomial & mask);
          }

          crc32_table[byte] = crc;
      }

      return crc32_table;
  }

  // Stores CRC-32 table and softly validates it.
  static constexpr auto crc32_table = gen_crc32_table();

  // Generates CRC-32 code from null-terminated, c-string,
  // algorithm based from this link:
  // http://www.hackersdelight.org/hdcodetxt/crc.c.txt
  constexpr auto crc32(const char *in) {
      auto crc = 0xFFFFFFFFu;

      for (auto i = 0u; auto c = in[i]; ++i) {
          crc = crc32_table[(crc ^ c) & 0xFF] ^ (crc >> 8);
      }

      return ~crc;
  }

  void dummy( uint32_t i )
  {
    uint32_t b = i;
    LOG( debug, b );
  }



  static int execute( int argc, char* argv[] )
  {
    (void)argc;
    (void)argv;

    typedef resource_manager< test_data > test_data_manager;
    typedef test_data_manager::resource resource;

    test_data_manager manager;
    manager.load( "dummy_file.thing", "dummy" );
    uint32_t dummy_id = hash_name( "dummy" );
    resource& res  = manager.get( dummy_id );
    resource& null = resource::null;

    res.dummy = 1;

    LOG( debug, "res  is null: " << res.is_null() );
    LOG( debug, "null is null: " << null.is_null() );
    LOG( debug, "")
    LOG( debug, "res  dummy: " << res.dummy );
    LOG( debug, "null dummy: " << null.dummy );

    dummy( crc32("test_me/man.conf") );

    return 0;
  }
}

int main( int argc, char* argv[] )
{
  return graphics_origin::execute( argc, argv );
}
