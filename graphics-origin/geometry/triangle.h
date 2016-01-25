/* Created on: Jan 25, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TRIANGLE_H_
# define GRAPHICS_ORIGIN_TRIANGLE_H_

# include "../graphics_origin.h"
# include "./concepts/box_intersecter.h"
# include "./vec.h"

BEGIN_GO_NAMESPACE
namespace geometry {
  class triangle:
      public box_intersecter {
  public:


  private:

    bool
    do_intersect( const aabox& b ) const override;

    vec3 vertices[3];
    vec3 normal;
  };

}
END_GO_NAMESPACE
# endif 
