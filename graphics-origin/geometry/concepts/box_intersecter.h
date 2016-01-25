# ifndef GRAPHICS_ORIGIN_BOX_INTERSECTER_H_
# define GRAPHICS_ORIGIN_BOX_INTERSECTER_H_
# include "../../graphics_origin.h"
BEGIN_GO_NAMESPACE namespace geometry {
  class aabox;
  class box_intersecter {
    virtual bool
    do_intersect(const aabox& b) const = 0;
  public:
    virtual ~box_intersecter() = default;
    bool
    intersect(const aabox& b) const
    {
      return do_intersect(b);
    }
  };
} END_GO_NAMESPACE
# endif
