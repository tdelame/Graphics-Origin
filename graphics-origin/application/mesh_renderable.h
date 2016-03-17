/*  Created on: Mar 16, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_MESH_RENDERABLE_H_
# define GRAPHICS_ORIGIN_MESH_RENDERABLE_H_
# include "../graphics_origin.h"
# include "renderable.h"
# include "../../graphics-origin/geometry/mesh.h"
namespace graphics_origin {
  namespace geometry {
    class mesh;
  }
  namespace application {
    class mesh_renderable
      : public renderable {
    public:
      mesh_renderable(
          shader_program_ptr program );
      ~mesh_renderable();
      void load( const std::string& filename );

      geometry::mesh& get_geometry()
      {
        return m_mesh;
      }

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      geometry::mesh m_mesh;

      unsigned int m_vao;
      enum{ POSITION_NORMAL, INDICES, NUMBER_OF_VBO };
      unsigned int m_vbo[ NUMBER_OF_VBO ];
    };
  }
}
# endif
