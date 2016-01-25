/* Created on: Jan 25, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/geometry/box.h>
# include <graphics-origin/geometry/triangle.h>
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
  rad = fa * box.get_half_sides()[ 1 ] + fb * box.get_half_sides()[ 2 ]; \
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
  rad = fa * box.get_half_sides()[ 1 ] + fb * box.get_half_sides()[ 2 ]; \
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
  rad = fa * box.get_half_sides()[ 0 ] + fb * box.get_half_sides()[ 2 ]; \
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
  rad = fa * box.get_half_sides()[ 0 ] + fb * box.get_half_sides()[ 2 ]; \
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
  rad = fa * box.get_half_sides()[ 0 ] + fb * box.get_half_sides()[ 1 ]; \
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
  rad = fa * box.get_half_sides()[ 0 ] + fb * box.get_half_sides()[ 1 ]; \
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
  triangle::do_intersect( const aabox& box ) const
  {
    vec3 v0 = vertices[0] - box.get_center();
    vec3 v1 = vertices[1] - box.get_center();
    vec3 v2 = vertices[2] - box.get_center();

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
    if(min>box.get_half_sides()[0] || max<-box.get_half_sides()[0]) return false;

    FINDMINMAX(v0[1],v1[1],v2[1],min,max);
    if(min>box.get_half_sides()[1] || max<-box.get_half_sides()[1]) return false;

    FINDMINMAX(v0[2],v1[2],v2[2],min,max);
    if(min>box.get_half_sides()[2] || max<-box.get_half_sides()[2]) return false;

    return plane_overlap_box( normal, vertices[0], box.get_half_sides() );
  }


}
END_GO_NAMESPACE
