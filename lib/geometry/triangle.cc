/* Created on: Jan 25, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/geometry/triangle.h"
# include "../../graphics-origin/geometry/ray.h"
BEGIN_GO_NAMESPACE
namespace geometry {

# define AXISTEST_X01( a, b, fa, fb )                                    \
  p[0] = a * v0[1] - b * v0[2];                                          \
  p[2] = a * v2[1] - b * v2[2];                                          \
  if( p[0] < p[2] )                                                      \
    {                                                                    \
      min = p[0];                                                        \
      max = p[2];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[2];                                                        \
      max = p[0];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 1 ] + fb * bb.m_hsides[ 2 ];   \
  if( min > rad || max < -rad )                                          \
    return false;

# define AXISTEST_X2( a, b, fa, fb )                                     \
  p[0] = a * v0[1] - b * v0[2];                                          \
  p[1] = a * v1[1] - b * v1[2];                                          \
  if( p[0] < p[1] )                                                      \
    {                                                                    \
      min = p[0];                                                        \
      max = p[1];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[1];                                                        \
      max = p[0];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 1 ] + fb * bb.m_hsides[ 2 ];   \
  if( min > rad || max < -rad )                                          \
    return false;


# define AXISTEST_Y02( a, b, fa, fb )                                    \
  p[0] = -a * v0[0] + b * v0[2];                                         \
  p[2] = -a * v2[0] + b * v2[2];                                         \
  if( p[0] < p[2] )                                                      \
    {                                                                    \
      min = p[0];                                                        \
      max = p[2];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[2];                                                        \
      max = p[0];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 0 ] + fb * bb.m_hsides[ 2 ];   \
  if( min > rad || max < -rad )                                          \
    return false;

# define AXISTEST_Y1( a, b, fa, fb )                                     \
  p[0] = -a * v0[0] + b * v0[2];                                         \
  p[1] = -a * v1[0] + b * v1[2];                                         \
  if( p[0] < p[1] )                                                      \
    {                                                                    \
      min = p[0];                                                        \
      max = p[1];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[1];                                                        \
      max = p[0];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 0 ] + fb * bb.m_hsides[ 2 ];   \
  if( min > rad || max < -rad )                                          \
    return false;

/**Z TESTS */
# define AXISTEST_Z12( a, b, fa, fb )                                    \
  p[1] = a * v1[0] - b * v1[1];                                          \
  p[2] = a * v2[0] - b * v2[1];                                          \
  if( p[1] < p[2] )                                                      \
    {                                                                    \
      min = p[1];                                                        \
      max = p[2];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[2];                                                        \
      max = p[1];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 0 ] + fb * bb.m_hsides[ 1 ];   \
  if( min > rad || max < -rad )                                          \
    return false;

# define AXISTEST_Z0( a, b, fa, fb )                                     \
  p[0] = a * v0[0] - b * v0[1];                                          \
  p[1] = a * v1[0] - b * v1[1];                                          \
  if( p[0] < p[1] )                                                      \
    {                                                                    \
      min = p[0];                                                        \
      max = p[1];                                                        \
    }                                                                    \
   else                                                                  \
    {                                                                    \
      min = p[1];                                                        \
      max = p[0];                                                        \
    }                                                                    \
  rad = fa * bb.m_hsides[ 0 ] + fb * bb.m_hsides[ 1 ];   \
  if( min > rad || max < -rad )                                          \
    return false;


# define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;                     \
  if(x1<min) min=x1;                  \
  if(x1>max) max=x1;                  \
  if(x2<min) min=x2;                  \
  if(x2>max) max=x2;

  static bool
  plane_overlap_box( const vec3& normal, const vec3& vertex, const vec3& half_sides )
  {
    vec3 vmin, vmax;
    for( int q = 0; q < 3; ++ q )
      {
        if( normal[q] > real(0) )
          {
            vmin[q] = -half_sides[q] - vertex[q];
            vmax[q] =  half_sides[q] - vertex[q];
          }
        else
          {
            vmin[q] =  half_sides[q] - vertex[q];
            vmax[q] = -half_sides[q] - vertex[q];
          }
      }

    if( dot( normal, vmin ) > real(0) )
      return false;
    return ( dot( normal, vmax ) >= real(0) );
  }


  bool
  triangle::intersect( const aabox& bb ) const
  {
    vec3 v0 = vertices[0] - bb.m_center;
    vec3 v1 = vertices[1] - bb.m_center;
    vec3 v2 = vertices[2] - bb.m_center;

    vec3 e0 = v1 - v0;
    vec3 e1 = v2 - v1;
    vec3 e2 = v0 - v2;

    vec3 p;
    real min, max, rad, fex, fey, fez;


    fex = std::abs( e0[0] );
    fey = std::abs( e0[1] );
    fez = std::abs( e0[2] );

    AXISTEST_X01(e0[2], e0[1], fez, fey);
    AXISTEST_Y02(e0[2], e0[0], fez, fex);
    AXISTEST_Z12(e0[1], e0[0], fey, fex);

    fex = std::abs(e1[0]);
    fey = std::abs(e1[1]);
    fez = std::abs(e1[2]);

    AXISTEST_X01(e1[2], e1[1], fez, fey);
    AXISTEST_Y02(e1[2], e1[0], fez, fex);
    AXISTEST_Z0 (e1[1], e1[0], fey, fex);

    fex = std::abs(e2[0]);
    fey = std::abs(e2[1]);
    fez = std::abs(e2[2]);

    AXISTEST_X2 (e2[2], e2[1], fez, fey);
    AXISTEST_Y1 (e2[2], e2[0], fez, fex);
    AXISTEST_Z12(e2[1], e2[0], fey, fex);


    FINDMINMAX(v0[0],v1[0],v2[0],min,max);
    if(min>bb.m_hsides[0] || max<-bb.m_hsides[0]) return false;

    FINDMINMAX(v0[1],v1[1],v2[1],min,max);
    if(min>bb.m_hsides[1] || max<-bb.m_hsides[1]) return false;

    FINDMINMAX(v0[2],v1[2],v2[2],min,max);
    if(min>bb.m_hsides[2] || max<-bb.m_hsides[2]) return false;

    return plane_overlap_box( normal, vertices[0], bb.m_hsides );
  }


  triangle::triangle(
      const vec3& p1,
      const vec3& p2,
      const vec3& p3 )
    : vertices{ p1, p2, p3 }, normal{ normalize(cross( p2 - p1, p3 - p1 )) }
  {}

  void
  triangle::compute_bounding_box( aabox& b ) const
  {
    b = aabox(
        min( min( vertices[0], vertices[1] ), vertices[2] ),
        max( max( vertices[0], vertices[1] ), vertices[2] ) );
  }

  triangle::triangle()
  {}

  triangle::triangle( const triangle& t )
    : vertices{ t.vertices[0], t.vertices[1], t.vertices[2] },
      normal{ t.normal }
  {}

  triangle&
  triangle::operator=( const triangle& t )
  {
    vertices[0] = t.vertices[0];
    vertices[1] = t.vertices[1];
    vertices[2] = t.vertices[2];
    normal = t.normal;
    return *this;
  }

  const vec3&
  triangle::get_vertex( vertex_index i ) const
  {
    return vertices[ i ];
  }

  bool
  triangle::intersect( const ray& r, real& t ) const
  {
    auto edge1 = vertices[1] - vertices[0];
    auto edge2 = vertices[2] - vertices[0];

    auto cross_dir_edge2 = cross( r.get_direction(), edge2 );
    auto determinant = dot( edge1, cross_dir_edge2 );
    if( std::abs( determinant ) < 1e-7 )
      return false;

    auto inv_determinant = real(1.0 /  determinant );
    auto v1_source = r.get_origin() - vertices[0];
    auto u = dot( v1_source, cross_dir_edge2 ) * inv_determinant;
    if( u < 0 || u > 1.0 ) return false;

    auto cross_v1_source_edge1 = cross( v1_source, edge1 );
    auto v = dot( r.get_direction(), cross_v1_source_edge1 ) * inv_determinant;
    if( v < 0 || v > 1.0 ) return false;

    t = dot( edge2, cross_v1_source_edge1 ) * inv_determinant;
    return t >= 0;
  }

  const vec3&
  triangle::get_normal() const noexcept
  {
    return normal;
  }

}
END_GO_NAMESPACE
