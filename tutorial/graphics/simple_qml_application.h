/*  Created on: Mar 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef TUTORIAL_SIMPLE_QML_APPLICATION_H_
# define TUTORIAL_SIMPLE_QML_APPLICATION_H_
# include "../../graphics-origin/application/gl_window_renderer.h"
# include <QtQuick/QQuickView>

namespace graphics_origin { namespace application {
  class simple_qml_application
    : public QQuickView {
    Q_OBJECT
  public:
    explicit simple_qml_application( QWindow* parent = nullptr );
    ~simple_qml_application();
  };
}}
# endif
