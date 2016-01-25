/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_BOUNDING_BOX_COMPUTER_H_
# define GRAPHICS_ORIGIN_BOUNDING_BOX_COMPUTER_H_
# include "../../graphics_origin.h"
BEGIN_GO_NAMESPACE namespace geometry {
  class aabox;
  class bounding_box_computer {
    virtual void
    do_compute_bounding_box( aabox& b ) const = 0;
  protected:
    ~bounding_box_computer() = default;
  public:
    void compute_bounding_box( aabox& b) const
    {
      do_compute_bounding_box( b );
    }
  };
} END_GO_NAMESPACE
# endif
