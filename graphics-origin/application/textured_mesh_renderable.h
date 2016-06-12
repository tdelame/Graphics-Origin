/*  Created on: Jun 10, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# ifndef TEXTURED_MESH_RENDERABLE_H_
# define TEXTURED_MESH_RENDERABLE_H_
# include "../graphics_origin.h"
# include "../geometry/mesh.h"
# include "renderable.h"

class FIBITMAP;

namespace graphics_origin {
  namespace application {
    /**@brief Render a static textured mesh.
     *
     * Render a textured mesh that should be static, or at least do not change
     * too often its attributes. The reason is that all attributes are interleaved
     * for better performance (see
     * www.opengl.org/wiki/Vertex_Specification_Best_Practices#Interleaving).
     */
    class GO_API textured_mesh_renderable:
        public renderable {
    public:
      /**
       * @param program
       */
      textured_mesh_renderable( shader_program_ptr program );
      ~textured_mesh_renderable();

      void load_mesh( const std::string& mesh_filename );
      void load_texture( const std::string& texture_filename );

      geometry::mesh& get_geometry();

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      geometry::mesh mesh;
      FIBITMAP* m_fib;
      enum { position_normal_texture_vbo, number_of_buffers };
      unsigned int m_vbo[ number_of_buffers ];
      unsigned int m_vao;
      unsigned int m_texture_id;
    };
}}
# endif /* TEXTURED_MESH_RENDERABLE_H_ */
