/*  Created on: Jan 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_CAMERA_H_
# define GRAPHICS_ORIGIN_CAMERA_H_
# include "../graphics_origin.h"
# include "../geometry/vec.h"
# include "../geometry/matrix.h"

# include <QtCore/QObject>
# include <QtGui/QVector3D>

namespace graphics_origin {
namespace application {

  class GO_API camera
    : public QObject {
    Q_OBJECT
    Q_PROPERTY( qreal fov READ get_fov WRITE set_fov )
    Q_PROPERTY( qreal znear READ get_znear WRITE set_znear )
    Q_PROPERTY( qreal zfar READ get_zfar WRITE set_zfar )
    Q_PROPERTY( qreal ratio READ get_ratio WRITE set_ratio )

    Q_PROPERTY( QVector3D position READ qml_get_position WRITE qml_set_position NOTIFY position_changed )
    Q_PROPERTY( QVector3D forward READ qml_get_forward WRITE qml_set_forward NOTIFY forward_changed )
    Q_PROPERTY( QVector3D right READ qml_get_right WRITE qml_set_right NOTIFY right_changed )
    Q_PROPERTY( QVector3D up READ qml_get_up WRITE qml_set_up NOTIFY up_changed )
  public:
    explicit camera( QObject* parent = nullptr );

    /**@brief Read access to the view matrix.
     *
     * Allow a read-only access to the view matrix, i.e. where the camera looks.
     * @return The view matrix
     */
    const gl_mat4& get_view_matrix() const;
    /**@brief Set the camera view matrix.
     *
     * Set the camera view matrix.
     * @param view The new view matrix
     */
    void set_view_matrix( const gl_mat4& view );

    /**@brief Read access to the camera world position.
     *
     * Allow a read-only access to the camera position in world coordinates.
     * @return The camera position.
     */
    gl_vec3 get_position() const;
    QVector3D qml_get_position() const;

    /**@brief Read access to the camera right direction.
     *
     * Allow a read-only access to the right direction of the camera in world
     * coordinates. This direction corresponds to the (+X) direction of the
     * camera local frame.
     *
     * @return The camera right direction
     */
    gl_vec3 get_right() const;
    QVector3D qml_get_right() const;

    /**@brief Read access to the camera up direction.
     *
     * Allow a read-only access to the up direction of the camera in world
     * coordinates. This direction corresponds to the (+Y) direction of the
     * camera local frame.
     *
     * @return The camera up direction.
     */
    gl_vec3 get_up() const;
    QVector3D qml_get_up() const;

    /**@brief Read access to the camera forward direction.
     *
     * Allow a read-only access to the forward direction of the camera in world
     * coordinates. This direction corresponds to the (-Z) direction of the
     * camera local frame, also known as the 'looking direction'.
     *
     * @return The camera forward direction.
     */
    gl_vec3 get_forward() const;
    QVector3D qml_get_forward() const;

    /**@brief Set the camera world position.
     *
     * Set the camera position in world coordinates.
     * @param pos New camera world position.
     */
    void set_position( const gl_vec3& pos );
    void qml_set_position( const QVector3D& pos );

    /**@brief Set the camera right direction.
     *
     * Set  the camera right direction in world coordinates.
     * @param right New camera right axis in world coordinates.
     */
    void set_right( const gl_vec3& right );
    void qml_set_right( const QVector3D& right );

    /**@brief Set the camera up direction.
     *
     * Set the camera up direction in world coordinates.
     * @param up New camera up axis in world coordinates.
     */
    void set_up( const gl_vec3& up );
    void qml_set_up( const QVector3D& up );

    /**@brief Set the camera up direction.
     *
     * Set the camera forward (-Z) direction in world coordinates.
     * @param forward New camera forward direction in world coordinates.
     */
    void set_forward( const gl_vec3& forward );
    void qml_set_forward( const QVector3D& forward );

    Q_INVOKABLE
    void arcball_rotate( qreal mouse_dx, qreal mouse_dy );

    Q_INVOKABLE
    void spaceship_rotate( qreal mouse_dx, qreal mouse_dy );
    ///@}


    /**
    * @name Camera Projection Matrix
    * Set of methods which interact with the projection matrix. This matrix
    * determine how a scene is projected on a 2D image that will be displayed
    * on screen.
    * @{
    */
    /**@brief Read access to the camera projection matrix.
     *
     * Allow a read-only access to the camera projection matrix. This matrix
     * defines how to go from the pixel coordinates to the screen coordinates.
     *
     * @return The projection matrix.
     */
    const gl_mat4& get_projection_matrix() const;

    /**@brief Get the camera field of view
     *
     * Get the field of view of the camera, also known as the camera angle.
     * @return The camera fov.
     */
    qreal get_fov() const;

    /**@brief Get the camera ratio.
     *
     * The camera ratio is the length ratio of the image taken by this camera. This
     * ratio is meant to be equal to the width of the display window divided by its
     * height. This ratio is also know as the aspect ratio.
     *
     * @return The length ratio of the image taken by this camera.
     */
    qreal get_ratio() const;

    /**@brief Get the near clipping plane.
     *
     * Get the near clipping plane: anything that is closer to the camera than this
     * will be removed from the rendered image.
     *
     * @return The near clipping plane distance.
     */
    qreal get_znear() const;

    /**@brief Get the far clipping plane.
     *
     * Get the far clipping plane: anything that is farther from the camera than this
     * plane will be removed from the rendered image.
     *
     * @return The far clipping plane distance.
     */
    qreal get_zfar() const;

    /**@brief Define the field of view.
     *
     * Set the camera field of view. We consider 1.04 is a good start to find a
     * custom fov.
     * @param v The new field of view.
     */
    void set_fov( const qreal& v );

    /**@brief Define the aspect ratio.
     *
     * Set the camera aspect ratio. Generally, this is done at each window resize.
     * The Viewer automatically call this method on its camera when its window is
     * resized.
     * @param v The new aspect ratio (width / height)
     */
    void set_ratio( const qreal& v );

    /**@brief Define the far clipping plane.
     *
     * Set the far clipping plane at the specified distance.
     * @param v The far clipping plane distance.
     */
    void set_zfar( const qreal& v );

    /**@brief Define the near clipping plane.
     *
     * Set the near clipping plane at the specified distance.
     * @param v The near clipping plane distance.
     */
    void set_znear( const qreal& v );
    ///@}

    void update();

  signals:
    void position_changed();
    void forward_changed();
    void right_changed();
    void up_changed();

  protected:

    virtual void do_update();
    qreal m_fov;
    qreal m_ratio;
    qreal m_znear;
    qreal m_zfar;
    gl_mat4 m_view;
    gl_mat4 m_projection;
  };

}
END_GO_NAMESPACE
# endif
