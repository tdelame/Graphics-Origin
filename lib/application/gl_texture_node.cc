/* Created on: Feb 2, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/gl_texture_node.h>
# include <graphics-origin/tools/log.h>
# include <QtQuick/QQuickWindow>
//force to expand the macro myself, as moc cannot do it
namespace graphics_origin {
namespace application {

  gl_texture_node::gl_texture_node(
      QQuickWindow *window) :
    m_identifier{ 0 }, m_size{ 0, 0 }, m_texture{ nullptr }, m_window{ window }
  {
    m_texture = m_window->createTextureFromId( m_identifier, QSize(1, 1));
    m_texture->bind();
    setTexture(m_texture);
    setFiltering(QSGTexture::Linear);
  }

  gl_texture_node::~gl_texture_node()
  {
    delete m_texture;
  }

  // This function gets called on the FBO rendering thread and will store the
  // texture id and size and schedule an update on the window.
  void
  gl_texture_node::new_texture(int id, const QSize &s)
  {
    m_mutex.lock();
    m_identifier = id;
    m_size = s;
    m_mutex.unlock();

    // We cannot call QQuickWindow::update directly here, as this is only allowed
    // from the rendering thread or GUI thread.
    emit pending_new_texture();
  }

  // Before the scene graph starts to render, we update to the pending texture
  void
  gl_texture_node::prepare_node()
  {
    m_mutex.lock();
      int newId = m_identifier;
      QSize t_size = m_size;
      m_identifier = 0;
    m_mutex.unlock();
    if( newId )
      {
        delete m_texture;
        m_texture = m_window->createTextureFromId(newId, t_size);
        setTexture(m_texture);

        markDirty(DirtyMaterial);

        // This will notify the rendering thread that the texture is now being rendered
        // and it can start rendering to the other one.
        emit texture_in_use();
      }
  }

}
END_GO_NAMESPACE
