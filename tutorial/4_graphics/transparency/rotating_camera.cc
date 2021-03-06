/* Created on: Jun 13, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "rotating_camera.h"
# include <omp.h>

namespace graphics_origin { namespace application {

  static const gl_vec3 initial_position = gl_vec3{4,0,0};
  static const gl_real rotation_speed = 2.0 * M_PI / 8.0;
  static const gl_mat4 initial_view = glm::lookAt( initial_position, gl_vec3{}, gl_vec3{0,0,1});

  rotating_camera::rotating_camera( QObject* parent )
    : graphics_origin::application::camera{ parent },
      m_last_update_time( omp_get_wtime() ),
      m_rotation_angle( 0 )
  {
    m_zfar = 7.0;
    m_znear = 0.01;
    m_projection = glm::perspective( m_fov, m_ratio, m_znear, m_zfar );

    m_view = initial_view;
  }

  void
  rotating_camera::do_update()
  {
    auto time = omp_get_wtime();
    m_rotation_angle += (time - m_last_update_time) * rotation_speed;
    m_last_update_time = time;

    m_view = glm::rotate( initial_view, m_rotation_angle, gl_vec3{0,0,1});
    set_position( - gl_vec3( m_view[3] ) * gl_mat3(m_view) );
    emit position_changed();
    emit forward_changed();
    emit right_changed();
  }
}}
