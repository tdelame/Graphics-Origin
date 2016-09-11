/* Created on: Jan 7, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_VEC_H_
# define GRAPHICS_ORIGIN_VEC_H_
# include "../graphics_origin.h"
# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp>

BEGIN_GO_NAMESPACE

typedef glm::tvec2<float, glm::highp> fvec2;
typedef glm::tvec3<float, glm::highp> fvec3;
typedef glm::tvec4<float, glm::highp> fvec4;

typedef glm::tvec2<unsigned int, glm::highp > uivec2;
typedef glm::tvec2<uint32_t, glm::highp > uint32_vec2;

typedef glm::tvec2<real, glm::highp> vec2;
typedef glm::tvec3<real, glm::highp> vec3;
typedef glm::tvec4<real, glm::highp> vec4;

/**
 * In OpenGL, gl_Position should have float components. Thus, if vec3 uses
 * double, we need to make conversions in every GLSL shader. Instead, we
 * have other vec types to transfer geometry on the GPU.
 */
typedef glm::tvec2<gl_real, glm::highp> gl_vec2;
typedef glm::tvec3<gl_real, glm::highp> gl_vec3;
typedef glm::tvec4<gl_real, glm::highp> gl_vec4;

/**
 * These using directives avoid to type glm::cross, glm::dot... every time.
 */
using glm::cross;
using glm::dot;
using glm::distance;
using glm::length;
using glm::normalize;
using glm::max;
using glm::min;

template< typename T >
T min( const glm::tvec2< T, glm::highp >& x  )
{
  return min( x.x, x.y );
}

template< typename T >
T min( const glm::tvec3< T, glm::highp >& x )
{
  return min( x.x, min( x.y, x.z ));
}

template< typename T >
T min( const glm::tvec4< T, glm::highp >& x )
{
  return min( min(x.x, x.y), min(x.z, x.w ));
}

template< typename T >
T max( const glm::tvec2< T, glm::highp >& x  )
{
  return max( x.x, x.y );
}

template< typename T >
T max( const glm::tvec3< T, glm::highp >& x )
{
  return max( x.x, max(x.y, x.z ));
}

template< typename T >
T max( const glm::tvec4< T, glm::highp >& x )
{
  return max( max(x.x, x.y), max(x.z, x.w) );
}


/** @brief Compute the square distance between two vec3
 *
 * This function computes the squared euclidean distance between two
 * 3D points.
 * @param a The first point
 * @param b The second point
 */
GO_API
real
sdistance( const vec3& a, const vec3& b );

/** @brief Compute the square distance between two vec2
 *
 * This function computes the squared euclidean distance between two
 * 2D points.
 * @param a The first point
 * @param b The second point
 */
GO_API
real
sdistance( const vec2& a, const vec2& b );

/** @brief Map a parameter to a color.
 *
 * Maps a parameter in the interval [low, high] to a color code. The cold colors
 * are for small parameter values, and warms colors are for high values.
 * @param parameter The parameter
 * @param low The lowest value for the parameters
 * @param high The highest value for the parameters
 */
GO_API
gl_vec3
get_color(
    const gl_real& parameter,
    const gl_real& low,
    const gl_real& high );

/** @brief Convert a RGB color to a gray scale color
 *
 * Convert a RGB color to a gray scale color.
 * @param color The RGB color to convert
 */
GO_API
gl_vec3
get_gray_scale( const gl_vec3& color );


END_GO_NAMESPACE

# include <iosfwd>

namespace std {

  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec2& obj);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec3& obj);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::vec4& obj);

  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec2& obj);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec3& obj);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::vec4& obj);

  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_vec2& obj);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_vec3& obj);
  GO_API std::ostream& operator<<(std::ostream& os, const GO_NAMESPACE::gl_vec4& obj);

  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_vec2& obj);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_vec3& obj);
  GO_API std::istream& operator>>(std::istream& is, GO_NAMESPACE::gl_vec4& obj);
}
#endif
