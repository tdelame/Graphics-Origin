// Here you import elements into your script. It can be seen as loading dynamic "libraries".
import QtQuick 2.5
import QtQuick.Controls 1.4
import GraphicsOrigin 1.0 // This "library" is defined from the C++ side and contains GLWindow and GLCamera.
import "." //QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
  id: main
  width: 1000
  height: 1000
  color: Style.main_window.background
  visible: true
  
  GLCamera {
    id: cam1
    ratio: main.height > 0 ? main.width / main.height : 1
  }
    
  GLWindow {
    id: glwindow
    width: main.width
    height: main.height
    anchors.centerIn: main
    camera: cam1
  }
    
  // Executed when everything is loaded
  Component.onCompleted: {
    cam1.znear = 0.01
    cam1.zfar  = 7
    cam1.position = Qt.vector3d( 4, 0, 0 )
    cam1.forward  = Qt.vector3d( -1, 0, 0 )
    cam1.up       = Qt.vector3d( 0, 0, 1 )
    cam1.right    = Qt.vector3d( 0, 1, 0 )
  }
  
  Text {
    id: fps
    anchors.top: parent.top
    anchors.right: parent.right
  }  
  
  Timer {
    interval: 2000; running: true; repeat: true
    onTriggered: {
      fps.text = "FPS: " + glwindow.get_fps().toFixed(2)
    }
  }
  
  Timer {
    interval: 15; running: true; repeat: true
    onTriggered: {
      cam1.arcball_rotate( 0.01, 0 );
    }
  }
}