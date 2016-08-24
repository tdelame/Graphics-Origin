/* Created on: 2015-08-15
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/matrix.h"
# include <iostream>
namespace std {

  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat3& v)
  {
    return os << '{' << v[0] << ',' << v[1] << ',' << v[2] << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::mat4& v)
  {
    return os << '{' << v[0] << ',' << v[1] << ',' << v[2] << ',' << v[3] << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_mat3& v)
  {
    return os << '{' << v[0] << ',' << v[1] << ',' << v[2] << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_mat4& v)
  {
    return os << '{' << v[0] << ',' << v[1] << ',' << v[2] << ',' << v[3] << '}';
  }

  std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat3& v)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', rbracket = '\0';
    is >> lbracket >> v[0] >> comma1 >> v[1] >> comma2 >> v[2] >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || rbracket != '}' )
      is.setstate( std::ios_base::failbit );
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::mat4& v)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', comma3 = '\0', rbracket = '\0';
    is >> lbracket >> v[0] >> comma1 >> v[1] >> comma2 >> v[2] >> comma3 >> v[3] >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || comma3 != ',' || rbracket != '}' )
      is.setstate( std::ios_base::failbit );
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_mat3& v)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', rbracket = '\0';
    is >> lbracket >> v[0] >> comma1 >> v[1] >> comma2 >> v[2] >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || rbracket != '}' )
      is.setstate( std::ios_base::failbit );
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_mat4& v)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', comma3 = '\0', rbracket = '\0';
    is >> lbracket >> v[0] >> comma1 >> v[1] >> comma2 >> v[2] >> comma3 >> v[3] >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || comma3 != ',' || rbracket != '}' )
      is.setstate( std::ios_base::failbit );
    return is;
  }
}
