/* Created on: Jun 13, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef PROJECT_ROTATING_CAMERA_H_
# define PROJECT_ROTATING_CAMERA_H_
# include "../../graphics-origin/application/camera.h"

namespace graphics_origin { namespace application {
  /**@brief A camera that rotate around the scene of the transparency demo.
   *
   * This camera simply rotate around the scene of the transparency demo. This
   * is done by adding angles to a variable at each update, in order to recompute
   * the whole view matrix with high precision.
   */
  class rotating_camera
    : public graphics_origin::application::camera {
    Q_OBJECT
  public:
    explicit rotating_camera( QObject* parent = nullptr );

  private:
    void do_update() override;
    gpu_real m_last_update_time;
    gpu_real m_rotation_angle;
  };
}}
# endif
