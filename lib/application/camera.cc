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


}
}
