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

  namespace detail {

  static constexpr uint32_t crc32_table[256] = {
       0x0, 0x77073096, 0xee0e612c, 0x990951ba,  0x76dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
 0xedb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,  0x9b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190,  0x1db7106, 0x98d220bc, 0xefd5102a, 0x71b18589,  0x6b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2,  0xf00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb,  0x86d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6,  0x3b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af,  0x4db2615, 0x73dc1683,
0xe3630b12, 0x94643b84,  0xd6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d,  0xa00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c,  0x26d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785,  0x5005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae,  0xcb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7,  0xbdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

    constexpr uint32_t crc32_recursive( const char* in, const uint32_t crc )
    {
      return *in ?  crc32_recursive( ++in, crc32_table[ ((*in)^crc) & 0xFF ] ^ (crc >> 8)) : crc;
    }
  }

  constexpr uint32_t crc32( const char* in )
  {
    return ~detail::crc32_recursive( in, 0xFFFFFFFFu );
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

    LOG( debug, "some-id = " << crc32("some-id"));

    dummy( crc32("test_me/man.conf") );

    return 0;
  }
}

int main( int argc, char* argv[] )
{
  return graphics_origin::execute( argc, argv );
}
