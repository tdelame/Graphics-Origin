/* Created on: Feb 2, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/tools/log.h"
namespace graphics_origin {
namespace application {

  camera::camera( QObject* parent )
    : QObject{ parent },
      m_fov{1.04}, m_ratio{1.0}, m_znear{0.05}, m_zfar{ 15},
      m_view{ glm::lookAt( gpu_vec3{3,0,0}, gpu_vec3{}, gpu_vec3{0,0,1})},
      m_projection{ glm::perspective( m_fov, m_ratio, m_znear, m_zfar ) }
  {}

  const gpu_mat4&
  camera::get_view_matrix() const
  {
    return m_view;
  }

  void
  camera::set_view_matrix( const gpu_mat4& view )
  {
    m_view = view;
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
    return gpu_vec3{ m_view[0][1], m_view[1][1], m_view[2][1] };
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
    return gpu_vec3{ -m_view[0][2], -m_view[1][2], -m_view[2][2] };
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
//
//    if( std::isnan( m_view[3][0] )
//     || std::isnan( m_view[3][1] )
//     || std::isnan( m_view[3][2] ) )
//    {
//        LOG( error, "IS NAN from " << pos );
//    }
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

  qreal camera::get_fov() const
  {
    return m_fov;
  }

  qreal camera::get_ratio() const
  {
    return m_ratio;
  }

  qreal camera::get_znear() const
  {
    return m_znear;
  }

  qreal camera::get_zfar() const
  {
    return m_zfar;
  }

  void camera::set_fov( const qreal& v )
  {
    m_fov = v;
    m_projection = glm::perspective( gpu_real(m_fov), gpu_real(m_ratio), gpu_real(m_znear), gpu_real(m_zfar) );
  }

  void camera::set_ratio( const qreal& v )
  {
    m_ratio = v;
    m_projection = glm::perspective( gpu_real(m_fov), gpu_real(m_ratio), gpu_real(m_znear), gpu_real(m_zfar) );
  }

  void camera::set_zfar( const qreal& v )
  {
    m_zfar = v;
    m_projection = glm::perspective( gpu_real(m_fov), gpu_real(m_ratio), gpu_real(m_znear), gpu_real(m_zfar) );
  }

  void camera::set_znear( const qreal& v )
  {
    m_znear = v;
    m_projection = glm::perspective( gpu_real(m_fov), gpu_real(m_ratio), gpu_real(m_znear), gpu_real(m_zfar) );
  }

  void camera::arcball_rotate( qreal mouse_dx, qreal mouse_dy )
  {
    gpu_mat4 rotation = glm::rotate( gpu_mat4( gpu_mat3( m_view ) ), gpu_real(mouse_dx), get_up() );
    rotation = glm::rotate( rotation, gpu_real(mouse_dy), get_right() );

    m_view[0] = rotation[0];
    m_view[1] = rotation[1];
    m_view[2] = rotation[2];

    set_position( - gpu_vec3( m_view[3] ) * gpu_mat3( rotation ) );

    emit position_changed();
    emit forward_changed();
    emit right_changed();
    emit up_changed();
  }

  void camera::spaceship_rotate( qreal mouse_dx, qreal mouse_dy )
  {
    const auto cx = std::cos( mouse_dx );
    const auto sx = std::sin( mouse_dx );
    const auto cy = std::cos( mouse_dy );
    const auto sy = std::sin( mouse_dy );

    gpu_mat3 rotation = gpu_mat3( m_view );
    gpu_vec3 minus_pos = gpu_vec3( m_view[3] ) * rotation;

//    if( std::isnan( minus_pos.x ) || std::isnan( minus_pos.y ) || std::isnan( minus_pos.z ) )
//      {
//        LOG( error, "minus pos is nan = " << minus_pos <<", dmouse = " << mouse_dx << ", " << mouse_dy <<", rotation = \n"
//             << rotation[0] << "\n" << rotation[1] << "\n" << rotation[2] << "\nm_view[3] = " << m_view[3] );
//      }



    // build a rotation matrix to apply to the current rotation:
    //
    rotation = glm::mat3(
                glm::vec3( cx, sx*sy,-sx*cy),
                glm::vec3(  0,    cy,    sy),
                glm::vec3( sx,-cx*sy, cx*cy) ) *rotation;




    for( int col = 0; col < 3; ++ col )
        for( int lin = 0; lin < 3; ++ lin )
        {
            m_view[col][lin] = rotation[col][lin];
        }

    set_position( -minus_pos );

//    LOG( debug, "m_view = \n" << m_view[0] << "\n" << m_view[1] << "\n" << m_view[2] << "\n" << m_view[3] );

    emit position_changed();
    emit forward_changed();
    emit right_changed();
    emit up_changed();
  }

  void camera::update()
  {
    do_update();
  }

  void camera::do_update()
  {}

}

}
