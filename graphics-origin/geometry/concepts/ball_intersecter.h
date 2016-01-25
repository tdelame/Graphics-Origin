# ifndef GRAPHICS_ORIGIN_BALL_INTERSECTER_H_
# define GRAPHICS_ORIGIN_BALL_INTERSECTER_H_

# include "../../graphics_origin.h"

BEGIN_GO_NAMESPACE namespace geometry {
  class ball;
  class ball_intersecter {
    virtual bool
    do_intersect(const ball& s) const = 0;
  protected:
    // no polymorphycal destruction allowed
    ~ball_intersecter()
    {}
  public:
    bool
    intersect(const ball& s) const
    {
      return do_intersect(s);
    }
  };
} END_GO_NAMESPACE
# endif
