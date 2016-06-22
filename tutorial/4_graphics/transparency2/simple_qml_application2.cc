/*  Created on: Mar 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# include "simple_qml_application2.h"

# include "../../../graphics-origin/application/qt_application/window.h"

# include <QtGui/QSurfaceFormat>

namespace graphics_origin { namespace application {

  simple_qml_application2::simple_qml_application2( QWindow* parent )
    : QQuickView( parent )
  {
    QSurfaceFormat format;
    format.setMajorVersion( 4 );
    format.setMinorVersion( 4 );
    format.setSwapInterval( 1 );
    setFormat( format );
    setResizeMode(QQuickView::SizeRootObjectToView );
    // Rendering in a thread introduces a slightly more complicated cleanup
    // so we ensure that no cleanup of graphics resources happen until the
    // application is shutting down.
    setPersistentOpenGLContext(true);
    setPersistentSceneGraph(true);
  }

  simple_qml_application2::~simple_qml_application2()
  {
    // As the render threads make use of our QGuiApplication object
    // to clean up gracefully, wait for them to finish before
    // QGuiApp is taken off the heap.
    foreach(graphics_origin::application::qt::window* w, graphics_origin::application::qt::window::window_instances)
      {
        w->pause();
        delete w;
      }
  }
}}
