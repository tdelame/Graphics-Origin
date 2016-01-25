# ifndef GRAPHICS_ORIGIN_BOX_CONTAINER_H_
# define GRAPHICS_ORIGIN_BOX_CONTAINER_H_
# include "../../graphics_origin.h"
BEGIN_GO_NAMESPACE namespace geometry {
  class aabox;
  class box_container {
    virtual bool
    do_contain(const aabox& b) const = 0;
  protected:
    ~box_container() = default;
  public:
    bool
    contain(const aabox& b) const
    {
      return do_contain(b);
    }
  };
} END_GO_NAMESPACE
# endif
