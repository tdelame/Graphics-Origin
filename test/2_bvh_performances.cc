/*  Created on: May 19, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# define GO_USE_OPENCL

# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/tools/resources.h"

# include "../../graphics-origin/tools/log.h"
# include "../../graphics-origin/tools/parallel.h"
# include "../../graphics-origin/geometry/traits.h"
# include "../../graphics-origin/geometry/box.h"
# include <vector>
# include <type_traits>
namespace graphics_origin {
  namespace geometry {

    // will wait to have openCL 2.1 to start
    static int execute()
    {
      const std::string mesh_dir = tools::get_path_manager().get_resource_directory( "meshes" );




      return EXIT_SUCCESS;
    }
  }
}
int main( int argc, char* argv[] )
{
  return graphics_origin::geometry::execute();
}



