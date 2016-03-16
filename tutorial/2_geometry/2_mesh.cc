/*  Created on: Mar 16, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/tools/log.h"

namespace graphics_origin {
  namespace geometry {
    static int execute()
    {
      mesh m;
      LOG( info, "sizeof a mesh point = " << sizeof( mesh::Point ) );
      LOG( info, "sizeof a float      = " << sizeof( float ) );
    }
  }
}

int main( int argc, char* argv[] )
{
  return graphics_origin::geometry::execute();
}
