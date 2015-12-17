/*  Created on: Dec 17, 2015
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_ANY_H_
# define GRAPHICS_ORIGIN_ANY_H_
# include <graphics_origin.h>
#   ifdef __CUDACC__
BEGIN_PROJECT_NAMESPACE namespace tools {
  typedef void* any;
} END_PROJECT_NAMESPACE
#   else

# include <string>
# include <typeindex>
# include <unordered_map>

# include <boost/spirit/home/support/detail/hold_any.hpp>
# include <boost/serialization/access.hpp>
# include <boost/archive/text_iarchive.hpp>
# include <boost/archive/text_oarchive.hpp>
# include <boost/archive/xml_oarchive.hpp>
# include <boost/archive/xml_iarchive.hpp>
# include <boost/archive/binary_oarchive.hpp>
# include <boost/archive/binary_iarchive.hpp>

BEGIN_GO_NAMESPACE namespace tools {
# define REGISTER_ANY_SERIALIZATION_TYPE( type, name )       \
  tools::get_type_to_name().insert(                          \
    std::make_pair(                                          \
      std::type_index(typeid( type )),                       \
      name ) );                                              \
  tools::get_name_to_serializer().insert(                    \
    std::make_pair(                                          \
      name,                                                  \
      new tools::detail::serialize_any_derived< type > () ) );

  struct any
     : public boost::spirit::hold_any
   {
     friend class boost::serialization::access;

     template<class archive>
     void save(archive & ar, const unsigned int version) const;

     template<class archive>
     void load( archive& ar, const unsigned int version );

     template <typename T>
     explicit any(T const& x);

     any()
       : boost::spirit::hold_any()
     {}

     any(any const& x )
       : boost::spirit::hold_any(x)
     {}

     ~any()
     {}

     template <typename T>
     any& operator=(T&& x)
     {
       assign(std::forward<T>(x));
       return *this;
     }

     BOOST_SERIALIZATION_SPLIT_MEMBER()
   };

  template< typename T >
  inline T* cast( any* operand )
  {
    return boost::spirit::any_cast<T>( operand );
  }

  template <typename T>
  inline T const* cast(any const* operand)
  {
    return boost::spirit::any_cast<T>( operand );
  }

  template <typename T>
  T const& cast(any const& operand)
  {
    return boost::spirit::any_cast<T>( operand );
  }

  namespace detail {
    struct serialize_any_base {
       virtual ~serialize_any_base(){}

       virtual void load( boost::archive::text_iarchive& ar, tools::any& any ) = 0;
       virtual void load( boost::archive::xml_iarchive& ar, tools::any& any ) = 0;
       virtual void load( boost::archive::binary_iarchive& ar, tools::any& any ) = 0;

       virtual void save( boost::archive::text_oarchive& ar, const tools::any& any ) = 0;
       virtual void save( boost::archive::xml_oarchive& ar, const tools::any& any ) = 0;
       virtual void save( boost::archive::binary_oarchive& ar, const tools::any& any ) = 0;
     };
  }

  extern std::unordered_map < std::type_index, std::string >& get_type_to_name();
  extern std::unordered_map < std::string, detail::serialize_any_base* >& get_name_to_serializer();

}
END_GO_NAMESPACE

# include <tools/detail/any.tcc>
# endif
# endif
