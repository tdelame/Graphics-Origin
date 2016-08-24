/*  Created on: Mar 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef TUTORIAL_SIMPLE_CAMERA_H_
# define TUTORIAL_SIMPLE_CAMERA_H_
# include "../../../graphics-origin/application/camera.h"

namespace graphics_origin { namespace application {

  class simple_camera
    : public graphics_origin::application::camera {
    Q_OBJECT
    Q_PROPERTY( qreal translation_speed READ qml_get_translation_speed WRITE qml_set_translation_speed )
  public:
    explicit simple_camera( QObject* parent = nullptr );

    Q_INVOKABLE void set_go_left( bool left );
    Q_INVOKABLE void set_go_right( bool right );
    Q_INVOKABLE void set_go_forward( bool forward);
    Q_INVOKABLE void set_go_backward( bool backward );
    Q_INVOKABLE void set_go_up( bool up );
    Q_INVOKABLE void set_go_down( bool down );
    Q_INVOKABLE void mouse_rotation( qreal dx, qreal dy );

    void set_translation_speed( gl_real speed );
    void qml_set_translation_speed( qreal speed );

    gl_real get_translation_speed() const;
    qreal qml_get_translation_speed() const;

  private:
    void do_update() override;

    gl_vec3 m_direction;
    real m_last_update_time;
    struct move {
      move();
      uint8_t forward : 1;
      uint8_t backward: 1;
      uint8_t left    : 1;
      uint8_t right   : 1;
      uint8_t up      : 1;
      uint8_t down    : 1;
    };
    move m_move;
    gl_real m_mouse_dx;
    gl_real m_mouse_dy;
    gl_real m_translation_speed;
    bool m_mouse_moved;
  };

}}
# endif
