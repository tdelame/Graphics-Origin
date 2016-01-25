/*  Created on: 8 nov. 2015
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_POINT_CONTAINER_H_
# define GRAPHICS_ORIGIN_POINT_CONTAINER_H_
# include "../../graphics_origin.h"
# include "../vec.h"
BEGIN_GO_NAMESPACE namespace geometry {
  class point_container {
    virtual bool
    do_contain(const vec3& p) const = 0;
  protected:
    ~point_container() = default;
  public:
    bool
    contain(const vec3& p) const
    {
      return do_contain(p);
    }
  };
} END_GO_NAMESPACE
# endif
