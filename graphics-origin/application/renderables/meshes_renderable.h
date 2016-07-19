# ifndef GRAPHICS_ORIGIN_MESHES_RENDERABLE_H_
# define GRAPHICS_ORIGIN_MESHES_RENDERABLE_H_
# include "../renderable.h"
# include "../../tools/tight_buffer_manager.h"

namespace graphics_origin {
  namespace geometry {
    class mesh;
  }
  namespace application {
    class GO_API meshes_renderable:
        public renderable {
      enum { position_normal_vbo, indices_vbo, number_of_buffers };
    public:

      struct storage {
        geometry::mesh* mesh;
        unsigned int buffer_ids[ number_of_buffers];
        unsigned int vao;
        bool dirty;
        bool active;
        bool destroyed;
        storage& operator=( storage&& other );
        storage();
        ~storage();
      };

    private:
      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > mesh_buffer;

    public:
      typedef mesh_buffer::handle handle;

      /**
       * @param program
       */
      meshes_renderable( shader_program_ptr program );
      ~meshes_renderable();

      handle add( const std::string& mesh_filename );
      void remove( handle h );
      storage& get( handle h );

      /**
       * TODO: managing render type (wireframe, color, transparency, ...)
       */

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      mesh_buffer m_meshes;
    };
}}
# endif 
