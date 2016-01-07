/*  Created on: Dec 17, 2015
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_ANY_H_
# define GRAPHICS_ORIGIN_ANY_H_
/** @file */
# include <graphics_origin.h>
#   ifdef __CUDACC__
BEGIN_GO_NAMESPACE namespace tools {
  typedef void* any;
} END_GO_NAMESPACE
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
/** Register a type for automatic serialization if stored in an any instance.
 * \code{.cpp}
 * struct foo {
 *   int i;
 *   int j;
 * };
 *
 * REGISTER_ANY_SERIALIZATION_TYPE( foo, "foo" )
 * \endcode
 */

# define REGISTER_ANY_SERIALIZATION_TYPE( type, name )       \
  tools::get_type_to_name().insert(                          \
    std::make_pair(                                          \
      std::type_index(typeid( type )),                       \
      name ) );                                              \
  tools::get_name_to_serializer().insert(                    \
    std::make_pair(                                          \
      name,                                                  \
      new tools::detail::serialize_any_derived< type > () ) );

  /** @class any
   * Provided that the stored type has the stream insertion operator << and
   * the stream extraction operator >> defined, this class allows to store any
   * value. Also, if the type had been registered with the macro
   * REGISTER_ANY_SERIALIZATION_TYPE, this class allows automatic serialization
   * for the following archives:
   * \li boost::archive::text_iarchive
   * \li boost::archive::xml_iarchive
   * \li boost::archive::binary_iarchive
   *
   * To get the stored value, you have to cast an any instance:
   * \code{.cpp}
   * tools::any var( int(1) );
   * var = tools::cast<int>(var) + tools::cast<int>(&var);
   * //var should contains now int(2)
   * \endcode
   */
  struct any
     : public boost::spirit::hold_any
   {
      /**@name Construction/Destruction*/
      /** @brief Explicit construction from value
       *
       * Construct an any instance storing a value of type T.
       * @param x The value to store in this
       */
      template <typename T>
      explicit any(T const& x);

      /** @brief Default construction
       *
       * Construct an empty any instance.
       */
      any()
        : boost::spirit::hold_any()
      {}

      /** @brief Copy construction
       *
       * Construct a copy of an any instance
       * @brief x The instance we want to copy into this
       */
      any(any const& x )
        : boost::spirit::hold_any(x)
      {}

      /** @brief Destruction
       */
      ~any()
      {}

      /** @name Assignment */
      /** @brief Assign a value
       *
       * Store a value of type T into this
       * @param x The value to store
       */
      template <typename T>
      any& operator=(T&& x)
      {
        assign(std::forward<T>(x));
        return *this;
      }

      /** @name Serialization interface */

      friend class boost::serialization::access;

      /** @brief Save to an archive
       *
       * Save this to an archive
       * @param ar The archive
       * @param version The storage version of the class any.
       */
      template<class archive>
      void save(archive & ar, const unsigned int version) const;

      /** @brief Load from an archive
       *
       * Load this from an archive
       * @param ar The archive
       * @param version The storage version of the class any
       */
      template<class archive>
      void load( archive& ar, const unsigned int version );

      BOOST_SERIALIZATION_SPLIT_MEMBER()
   };

  /** Cast an any into another type (by pointer) */
  template< typename T >
  inline T* cast( any* operand )
  {
    return boost::spirit::any_cast<T>( operand );
  }

  /** Cast an any into another type (by const pointer) */
  template <typename T>
  inline T const* cast(any const* operand)
  {
    return boost::spirit::any_cast<T>( operand );
  }

  /** Cast an any into another type (by const reference) */
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
