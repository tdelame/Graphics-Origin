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

}}
