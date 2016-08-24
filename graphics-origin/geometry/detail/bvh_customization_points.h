# include "../box.h"
# include "../ball.h"
# include "../triangle.h"

namespace graphics_origin {
namespace geometry {

  template<>
  struct bounding_volume_computer< aabox, triangle > {

    static void compute( const triangle& element, aabox& volume )
    {
      vec3 lower = min(
          min( element.get_vertex(triangle::V0), element.get_vertex(triangle::V1) ),
          element.get_vertex(triangle::V2) );

      vec3 upper = max(
          max( element.get_vertex(triangle::V0), element.get_vertex(triangle::V1) ),
          element.get_vertex(triangle::V2) );

      volume.center = real(0.5) * (lower + upper);
      volume.hsides = upper - volume.center;
    }

  };

  template<>
  struct bounding_volume_computer< aabox, ball > {

    static void compute( const ball& element, aabox& volume )
    {
      volume.center = vec3{element};
      volume.hsides.x = element.w;
      volume.hsides.y = element.w;
      volume.hsides.z = element.w;
    }
  };

  template<>
  struct bounding_volume_computer< ball, ball > {

    static void compute( const ball& element, ball& volume )
    {
      volume = element;
    }

  };

  template<>
  struct bounding_volume_analyzer<aabox> {

    static vec3 compute_lower_corner( const aabox& volume )
    {
      return volume.get_min();
    }

    static vec3 compute_upper_corner( const aabox& volume )
    {
      return volume.get_max();
    }

    static vec3 compute_center( const aabox& volume )
    {
      return volume.center;
    }
  };

  template<>
  struct bounding_volume_analyzer<ball> {

    static vec3 compute_lower_corner( const ball& volume )
    {
      return vec3{volume} - volume.w;
    }

    static vec3 compute_upper_corner( const ball& volume )
    {
      return vec3{volume} + volume.w;
    }

    static vec3 compute_center( const ball& volume )
    {
      return vec3{volume};
    }
  };

  template<>
  struct bounding_volume_merger<ball> {

    static ball merge( const ball& a, const ball& b )
    {
      /** We need to handle the cases where one of the ball is included into
       * the other one. To do so, let us first consider the case where there
       * are no inclusion. In such case, the longest line to include inside
       * the result is of length D = d + b.w + a.w,
       * with d = distance( vec3(a), vec3(b) ).
       * The minimal inclusive ball is then of radii 0.5D and the center
       * is located at vec3(a) + 0.5D - a.w.
       *
       * Now, let's deal with inclusions.
       * a is included inside b iff d + a.w < b.w <=> D < 2 b.w <=> 0.5D < b.w
       * b is included inside b iff d + b.w < a.w <=> D < 2 a.w <=> 0.5D < a.w
       *
       * Thus, in any case, we can start to compute the value 0.5D = radius.
       */
      real radius = a.w + b.w + distance(vec3(a), vec3(b));
      if( radius < a.w )
        return a;
      else if ( radius < b.w )
        return b;
      return ball{ vec3(a) + radius - a.w, radius };
    }
  };

  template<>
  struct bounding_volume_merger<aabox> {

    static aabox merge( const aabox& a, const aabox& b )
    {
      return aabox{
        min( a.get_min(), b.get_min() ),
        max( a.get_max(), b.get_max() )
      };
    }
  };

}}
