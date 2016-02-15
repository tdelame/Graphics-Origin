/* Created on: Feb 2, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/camera.h>
namespace graphics_origin {
namespace application {

  camera::camera( QObject* parent )
    : QObject{ parent },
      m_fov{}, m_ratio{}, m_znear{}, m_zfar{},
      m_view{},
      m_projection{}
  {}

  const gpu_mat4&
  camera::get_view_matrix() const
  {
    return m_view;
  }

  gpu_vec3
  camera::get_position() const
  {
    return -gpu_vec3( m_view[3] ) * gpu_mat3( m_view );
  }

  QVector3D
  camera::qml_get_position() const
  {
    auto pos = get_position();
    return QVector3D( pos.x, pos.y, pos.z );
  }

  gpu_vec3
  camera::get_right() const
  {
    return gpu_vec3{ m_view[0][0], m_view[1][0], m_view[2][0] };
  }

  QVector3D
  camera::qml_get_right() const
  {
    auto right = get_right();
    return QVector3D( right.x, right.y, right.z );
  }

  gpu_vec3
  camera::get_up() const
  {
    return gpu_vec3{ m_view[1][0], m_view[1][1], m_view[2][1] };
  }

  QVector3D
  camera::qml_get_up() const
  {
    auto up = get_up();
    return QVector3D( up.x, up.y, up.z );
  }

  gpu_vec3
  camera::get_forward() const
  {
    return gpu_vec3{ -m_view[2][0], -m_view[1][2], -m_view[2][2] };
  }

  QVector3D
  camera::qml_get_forward() const
  {
    auto forward = get_forward();
    return QVector3D( forward.x, forward.y, forward.z );
  }

  void
  camera::set_position( const gpu_vec3& pos )
  {
    for( int i = 0; i < 3; ++ i )
      {
        m_view[3][i] = gpu_real(0);
        for( int j = 0; j < 3; ++ j )
          m_view[3][i] -= m_view[j][i] * pos[j];
      }
  }

  void
  camera::qml_set_position( const QVector3D& pos )
  {
    for( int i = 0; i < 3; ++ i )
      {
        m_view[3][i] = gpu_real(0);
        for( int j = 0; j < 3; ++ j )
          m_view[3][i] -= m_view[j][i] * pos[j];
      }
  }

  void camera::set_right( const gpu_vec3& right )
  {
      gpu_vec3 minus_pos = gpu_vec3( m_view[3] ) * gpu_mat3( m_view );
      m_view[0][0] = right.x;
      m_view[1][0] = right.y;
      m_view[2][0] = right.z;
      m_view[3][0] = dot( right, minus_pos );
  }

  void camera::qml_set_right( const QVector3D& right )
  {
    set_right( gpu_vec3{ right[0], right[1], right[2] } );
  }

  void camera::set_up( const gpu_vec3& up )
  {
    gpu_vec3 minus_pos = gpu_vec3( m_view[3] ) * gpu_mat3( m_view );
    m_view[0][1] = up.x;
    m_view[1][1] = up.y;
    m_view[2][1] = up.z;
    m_view[3][1] = dot( up, minus_pos );
  }

  void camera::qml_set_up( const QVector3D& up )
  {
    set_up( gpu_vec3{ up[0], up[1], up[2] } );
  }

  void camera::set_forward( const gpu_vec3& forward )
  {
    gpu_vec3 pos = -gpu_vec3( m_view[3] ) * gpu_mat3( m_view );
    m_view[0][0] = -forward.x;
    m_view[1][0] = -forward.y;
    m_view[2][0] = -forward.z;
    m_view[3][0] = dot( forward, pos );
  }

  void camera::qml_set_forward( const QVector3D& forward )
  {
    set_forward( gpu_vec3{ forward[0], forward[1], forward[2] } );
  }

  const gpu_mat4&
  camera::get_projection_matrix() const
  {
    return m_projection;
  }

  gpu_real camera::get_fov() const
  {
    return m_fov;
  }

  gpu_real camera::get_ratio() const
  {
    return m_ratio;
  }

  gpu_real camera::get_znear() const
  {
    return m_znear;
  }

  gpu_real camera::get_zfar() const
  {
    return m_zfar;
  }

  void camera::set_fov( const gpu_real& v )
  {
    m_fov = v;
    m_projection = glm::perspective( m_fov, m_ratio, m_znear, m_zfar );
  }

  void camera::set_ratio( const gpu_real& v )
  {
    m_ratio = v;
    m_projection = glm::perspective( m_fov, m_ratio, m_znear, m_zfar );
  }

  void camera::set_zfar( const gpu_real& v )
  {
    m_zfar = v;
    m_projection = glm::perspective( m_fov, m_ratio, m_znear, m_zfar );
  }

  void camera::set_znear( const gpu_real& v )
  {
    m_znear = v;
    m_projection = glm::perspective( m_fov, m_ratio, m_znear, m_zfar );
  }

}

}
