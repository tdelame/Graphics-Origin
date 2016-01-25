# include "../log.h"
BEGIN_GO_NAMESPACE
namespace tools {
  namespace detail {

    template< class archive, class T >
    struct load_any {
      static void serialize( archive& ar, tools::any& any )
      {
        T temp;
        ar & boost::serialization::make_nvp( "data", temp );
        any = temp;
      }
    };

    template< class archive, class T >
    struct save_any {
      static void serialize( archive& ar, const tools::any& any )
      {
        ar & boost::serialization::make_nvp( "data", tools::cast<T>(any) );
      }
    };

# define IMPLEMENT_LOAD_ARCHIVE( archive_name )       \
  void load( archive_name& ar, tools::any& any )       \
  {                                                   \
    load_any<archive_name,T>::serialize( ar, any );   \
  }

# define IMPLEMENT_SAVE_ARCHIVE( archive_name )       \
  void save( archive_name& ar, const tools::any& any ) \
  {                                                   \
    save_any<archive_name,T>::serialize( ar, any );   \
  }

    template< typename T >
    struct serialize_any_derived
      : public serialize_any_base {

      IMPLEMENT_LOAD_ARCHIVE( boost::archive::text_iarchive )
      IMPLEMENT_LOAD_ARCHIVE( boost::archive::xml_iarchive )
      IMPLEMENT_LOAD_ARCHIVE( boost::archive::binary_iarchive )

      IMPLEMENT_SAVE_ARCHIVE( boost::archive::text_oarchive )
      IMPLEMENT_SAVE_ARCHIVE( boost::archive::xml_oarchive )
      IMPLEMENT_SAVE_ARCHIVE( boost::archive::binary_oarchive )
    };

# undef IMPLEMENT_LOAD_ARCHIVE
# undef IMPLEMENT_SAVE_ARCHIVE
  }

  template <typename T>
  any::any(T const& x)
    : boost::spirit::hold_any(x)
  {
  }

  template<class archive>
  void any::save(archive & ar, const unsigned int version) const
  {
    static const std::string empty_string = "_empty_";
    if( empty() )
      {
        ar & boost::serialization::make_nvp( "type_name", empty_string );
        ar & boost::serialization::make_nvp( "data", version );
        return;
      }

    auto search = get_type_to_name().find( type() );
    if( search == get_type_to_name().end() )
      {
        LOG(warning, "unknown type to serialize");
        ar & boost::serialization::make_nvp( "type_name", empty_string );
        ar & boost::serialization::make_nvp( "data", version );
        return;
      }
    ar & boost::serialization::make_nvp("type_name", search->second);
    get_name_to_serializer()[ search->second ]->save( ar, *this );
  }

  template<class archive>
  void any::load( archive& ar, const unsigned int version )
  {
    (void)version;
    std::string type_name;
    ar & boost::serialization::make_nvp("type_name", type_name);
    if( type_name == "empty" )
      {
        unsigned int dummy;
        ar & boost::serialization::make_nvp("data",dummy);
        reset();
        return;
      }

    auto search = get_name_to_serializer().find( type_name );
    if( search == get_name_to_serializer().end() )
      {
        LOG(error, "type [" << type_name << "] is not known for serialization of tools::any");
        unsigned int dummy;
        ar & boost::serialization::make_nvp("data",dummy);
        reset();
        return;
      }
    search->second->load( ar, *this );
  }
}
END_GO_NAMESPACE
