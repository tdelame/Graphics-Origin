/* Created on: 2015-08-15
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/geometry/vec.h>
# include <iostream>

BEGIN_GO_NAMESPACE
  static gpu_real LUT_Color [ 256 ][ 3 ] =
  {
          {        0,        0,   0.5156 },
          {        0,        0,   0.5313 },
          {        0,        0,   0.5469 },
          {        0,        0,   0.5625 },
          {        0,        0,   0.5781 },
          {        0,        0,   0.5938 },
          {        0,        0,   0.6094 },
          {        0,        0,    0.625 },
          {        0,        0,   0.6406 },
          {        0,        0,   0.6563 },
          {        0,        0,   0.6719 },
          {        0,        0,   0.6875 },
          {        0,        0,   0.7031 },
          {        0,        0,   0.7188 },
          {        0,        0,   0.7344 },
          {        0,        0,     0.75 },
          {        0,        0,   0.7656 },
          {        0,        0,   0.7813 },
          {        0,        0,   0.7969 },
          {        0,        0,   0.8125 },
          {        0,        0,   0.8281 },
          {        0,        0,   0.8438 },
          {        0,        0,   0.8594 },
          {        0,        0,    0.875 },
          {        0,        0,   0.8906 },
          {        0,        0,   0.9063 },
          {        0,        0,   0.9219 },
          {        0,        0,   0.9375 },
          {        0,        0,   0.9531 },
          {        0,        0,   0.9688 },
          {        0,        0,   0.9844 },
          {        0,        0,        1 },
          {        0,   0.0156,        1 },
          {        0,   0.0313,        1 },
          {        0,   0.0469,        1 },
          {        0,   0.0625,        1 },
          {        0,   0.0781,        1 },
          {        0,   0.0938,        1 },
          {        0,   0.1094,        1 },
          {        0,    0.125,        1 },
          {        0,   0.1406,        1 },
          {        0,   0.1563,        1 },
          {        0,   0.1719,        1 },
          {        0,   0.1875,        1 },
          {        0,   0.2031,        1 },
          {        0,   0.2188,        1 },
          {        0,   0.2344,        1 },
          {        0,     0.25,        1 },
          {        0,   0.2656,        1 },
          {        0,   0.2813,        1 },
          {        0,   0.2969,        1 },
          {        0,   0.3125,        1 },
          {        0,   0.3281,        1 },
          {        0,   0.3438,        1 },
          {        0,   0.3594,        1 },
          {        0,    0.375,        1 },
          {        0,   0.3906,        1 },
          {        0,   0.4063,        1 },
          {        0,   0.4219,        1 },
          {        0,   0.4375,        1 },
          {        0,   0.4531,        1 },
          {        0,   0.4688,        1 },
          {        0,   0.4844,        1 },
          {        0,      0.5,        1 },
          {        0,   0.5156,        1 },
          {        0,   0.5313,        1 },
          {        0,   0.5469,        1 },
          {        0,   0.5625,        1 },
          {        0,   0.5781,        1 },
          {        0,   0.5938,        1 },
          {        0,   0.6094,        1 },
          {        0,    0.625,        1 },
          {        0,   0.6406,        1 },
          {        0,   0.6563,        1 },
          {        0,   0.6719,        1 },
          {        0,   0.6875,        1 },
          {        0,   0.7031,        1 },
          {        0,   0.7188,        1 },
          {        0,   0.7344,        1 },
          {        0,     0.75,        1 },
          {        0,   0.7656,        1 },
          {        0,   0.7813,        1 },
          {        0,   0.7969,        1 },
          {        0,   0.8125,        1 },
          {        0,   0.8281,        1 },
          {        0,   0.8438,        1 },
          {        0,   0.8594,        1 },
          {        0,    0.875,        1 },
          {        0,   0.8906,        1 },
          {        0,   0.9063,        1 },
          {        0,   0.9219,        1 },
          {        0,   0.9375,        1 },
          {        0,   0.9531,        1 },
          {        0,   0.9688,        1 },
          {        0,   0.9844,        1 },
          {        0,        1,        1 },
          {   0.0156,        1,        1 },
          {   0.0313,        1,   0.9844 },
          {   0.0469,        1,   0.9688 },
          {   0.0625,        1,   0.9531 },
          {   0.0781,        1,   0.9375 },
          {   0.0938,        1,   0.9219 },
          {   0.1094,        1,   0.9063 },
          {    0.125,        1,   0.8906 },
          {   0.1406,        1,    0.875 },
          {   0.1563,        1,   0.8594 },
          {   0.1719,        1,   0.8438 },
          {   0.1875,        1,   0.8281 },
          {   0.2031,        1,   0.8125 },
          {   0.2188,        1,   0.7969 },
          {   0.2344,        1,   0.7813 },
          {     0.25,        1,   0.7656 },
          {   0.2656,        1,     0.75 },
          {   0.2813,        1,   0.7344 },
          {   0.2969,        1,   0.7188 },
          {   0.3125,        1,   0.7031 },
          {   0.3281,        1,   0.6875 },
          {   0.3438,        1,   0.6719 },
          {   0.3594,        1,   0.6563 },
          {    0.375,        1,   0.6406 },
          {   0.3906,        1,    0.625 },
          {   0.4063,        1,   0.6094 },
          {   0.4219,        1,   0.5938 },
          {   0.4375,        1,   0.5781 },
          {   0.4531,        1,   0.5625 },
          {   0.4688,        1,   0.5469 },
          {   0.4844,        1,   0.5313 },
          {      0.5,        1,   0.5156 },
          {   0.5156,        1,      0.5 },
          {   0.5313,        1,   0.4844 },
          {   0.5469,        1,   0.4688 },
          {   0.5625,        1,   0.4531 },
          {   0.5781,        1,   0.4375 },
          {   0.5938,        1,   0.4219 },
          {   0.6094,        1,   0.4063 },
          {    0.625,        1,   0.3906 },
          {   0.6406,        1,    0.375 },
          {   0.6563,        1,   0.3594 },
          {   0.6719,        1,   0.3438 },
          {   0.6875,        1,   0.3281 },
          {   0.7031,        1,   0.3125 },
          {   0.7188,        1,   0.2969 },
          {   0.7344,        1,   0.2813 },
          {     0.75,        1,   0.2656 },
          {   0.7656,        1,     0.25 },
          {   0.7813,        1,   0.2344 },
          {   0.7969,        1,   0.2188 },
          {   0.8125,        1,   0.2031 },
          {   0.8281,        1,   0.1875 },
          {   0.8438,        1,   0.1719 },
          {   0.8594,        1,   0.1563 },
          {    0.875,        1,   0.1406 },
          {   0.8906,        1,    0.125 },
          {   0.9063,        1,   0.1094 },
          {   0.9219,        1,   0.0938 },
          {   0.9375,        1,   0.0781 },
          {   0.9531,        1,   0.0625 },
          {   0.9688,        1,   0.0469 },
          {   0.9844,        1,   0.0313 },
          {        1,        1,   0.0156 },
          {        1,        1,        0 },
          {        1,   0.9844,        0 },
          {        1,   0.9688,        0 },
          {        1,   0.9531,        0 },
          {        1,   0.9375,        0 },
          {        1,   0.9219,        0 },
          {        1,   0.9063,        0 },
          {        1,   0.8906,        0 },
          {        1,    0.875,        0 },
          {        1,   0.8594,        0 },
          {        1,   0.8438,        0 },
          {        1,   0.8281,        0 },
          {        1,   0.8125,        0 },
          {        1,   0.7969,        0 },
          {        1,   0.7813,        0 },
          {        1,   0.7656,        0 },
          {        1,     0.75,        0 },
          {        1,   0.7344,        0 },
          {        1,   0.7188,        0 },
          {        1,   0.7031,        0 },
          {        1,   0.6875,        0 },
          {        1,   0.6719,        0 },
          {        1,   0.6563,        0 },
          {        1,   0.6406,        0 },
          {        1,    0.625,        0 },
          {        1,   0.6094,        0 },
          {        1,   0.5938,        0 },
          {        1,   0.5781,        0 },
          {        1,   0.5625,        0 },
          {        1,   0.5469,        0 },
          {        1,   0.5313,        0 },
          {        1,   0.5156,        0 },
          {        1,      0.5,        0 },
          {        1,   0.4844,        0 },
          {        1,   0.4688,        0 },
          {        1,   0.4531,        0 },
          {        1,   0.4375,        0 },
          {        1,   0.4219,        0 },
          {        1,   0.4063,        0 },
          {        1,   0.3906,        0 },
          {        1,    0.375,        0 },
          {        1,   0.3594,        0 },
          {        1,   0.3438,        0 },
          {        1,   0.3281,        0 },
          {        1,   0.3125,        0 },
          {        1,   0.2969,        0 },
          {        1,   0.2813,        0 },
          {        1,   0.2656,        0 },
          {        1,     0.25,        0 },
          {        1,   0.2344,        0 },
          {        1,   0.2188,        0 },
          {        1,   0.2031,        0 },
          {        1,   0.1875,        0 },
          {        1,   0.1719,        0 },
          {        1,   0.1563,        0 },
          {        1,   0.1406,        0 },
          {        1,    0.125,        0 },
          {        1,   0.1094,        0 },
          {        1,   0.0938,        0 },
          {        1,   0.0781,        0 },
          {        1,   0.0625,        0 },
          {        1,   0.0469,        0 },
          {        1,   0.0313,        0 },
          {        1,   0.0156,        0 },
          {        1,        0,        0 },
          {   0.9844,        0,        0 },
          {   0.9688,        0,        0 },
          {   0.9531,        0,        0 },
          {   0.9375,        0,        0 },
          {   0.9219,        0,        0 },
          {   0.9063,        0,        0 },
          {   0.8906,        0,        0 },
          {    0.875,        0,        0 },
          {   0.8594,        0,        0 },
          {   0.8438,        0,        0 },
          {   0.8281,        0,        0 },
          {   0.8125,        0,        0 },
          {   0.7969,        0,        0 },
          {   0.7813,        0,        0 },
          {   0.7656,        0,        0 },
          {     0.75,        0,        0 },
          {   0.7344,        0,        0 },
          {   0.7188,        0,        0 },
          {   0.7031,        0,        0 },
          {   0.6875,        0,        0 },
          {   0.6719,        0,        0 },
          {   0.6563,        0,        0 },
          {   0.6406,        0,        0 },
          {    0.625,        0,        0 },
          {   0.6094,        0,        0 },
          {   0.5938,        0,        0 },
          {   0.5781,        0,        0 },
          {   0.5625,        0,        0 },
          {   0.5469,        0,        0 },
          {   0.5313,        0,        0 },
          {   0.5156,        0,        0 }
  };

  real
  sdistance( const vec3& a, const vec3& b )
  {
    auto diff = b - a;
    return dot( diff, diff );
  }

  gpu_vec3
  get_color ( const gpu_real& factor, const gpu_real& low, const gpu_real& high )
  {
    float t = std::max ( low, std::min( factor, high) );
    t -= low;
    t /= (high - low );
    if ( t != t )
      {
        t = factor - low;
      }

    size_t indiceLut=floor(t * 255);
    return gpu_vec3 (LUT_Color[indiceLut][0],LUT_Color[indiceLut][1],LUT_Color[indiceLut][2]);
  }

  gpu_vec3
  get_gray_scale( const gpu_vec3& color )
  {
    gpu_real luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return gpu_vec3( luminance, luminance, luminance );
  }
END_GO_NAMESPACE

namespace std {

  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec2& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec3& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << ',' << obj.z << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec4& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << ',' << obj.z << ',' << obj.w << '}';
  }

  std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec2& obj)
  {
    char lbracket = '\0', comma1 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> rbracket;
    if( lbracket != '{' || comma1 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec3& obj)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> comma2 >> obj.z >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec4& obj)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', comma3 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> comma2 >> obj.z >> comma3 >> obj.w >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || comma3 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gpu_vec2& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gpu_vec3& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << ',' << obj.z << '}';
  }
  std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gpu_vec4& obj)
  {
    return os << '{' << obj.x << ',' << obj.y << ',' << obj.z << ',' << obj.w << '}';
  }

  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gpu_vec2& obj)
  {
    char lbracket = '\0', comma1 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> rbracket;
    if( lbracket != '{' || comma1 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gpu_vec3& obj)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> comma2 >> obj.z >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }
  std::istream& operator>>(std::istream& is, GO_NAMESPACE::gpu_vec4& obj)
  {
    char lbracket = '\0', comma1 = '\0', comma2 = '\0', comma3 = '\0', rbracket = '\0';
    is >> lbracket >> obj.x >> comma1 >> obj.y >> comma2 >> obj.z >> comma3 >> obj.w >> rbracket;
    if( lbracket != '{' || comma1 != ',' || comma2 != ',' || comma3 != ',' || rbracket != '}' )
      is.setstate(std::ios_base::failbit);
    return is;
  }
}
