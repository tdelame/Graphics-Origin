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
    translation_speed : 1.5
  }
    
  GLWindow {
    id: glwindow
    width: main.width
    height: main.height
    anchors.centerIn: main
    camera: cam1
    
    property bool move_fast : false
    property bool move_human : true
    
    
    focus: true
    Keys.onPressed: {
      if( event.key == Qt.Key_Q ) {
        cam1.set_go_left( true );
        event.accepted = true;
      } else if( event.key == Qt.Key_D ) {
        cam1.set_go_right( true );
		event.accepted = true;
      } else if( event.key == Qt.Key_Z ) {
        cam1.set_go_forward( true );
        event.accepted = true;
      } else if( event.key == Qt.Key_S ) {
        cam1.set_go_backward( true );
        event.accepted = true
  	  } else if( event.key == Qt.Key_Space ) {
  	    move_fast = true
  	    event.accepted = true
  	    cam1.translation_speed = move_human ? 10 : 4000
  	  } else if( event.key == Qt.Key_Shift ) {
  	    move_human = false
  	    event.accepted = true
  	    cam1.translation_speed = move_fast ? 4000 : 2000
  	  }
    }
       
    Keys.onReleased: {
      if( event.key == Qt.Key_Q ) {
        cam1.set_go_left( false );
        event.accepted = true;
      } else if( event.key == Qt.Key_D ) {
        cam1.set_go_right( false );
		event.accepted = true;
      } else if( event.key == Qt.Key_Z ) {
        cam1.set_go_forward( false );
        event.accepted = true;
      } else if( event.key == Qt.Key_S ) {
        cam1.set_go_backward( false );
        event.accepted = true
  	  } else if( event.key == Qt.Key_Space ) {
  	    move_fast = false
  	    event.accepted = true
  	    cam1.translation_speed = move_human ? 1.5 : 2000
  	  } else if( event.key == Qt.Key_Shift ) {
  	    move_human = true
  	    event.accepted = true
  	    cam1.translation_speed = move_fast ? 10 : 1.5
  	  }
    }
    
    MouseArea {
      anchors.fill:parent
      acceptedButtons: Qt.LeftButton | Qt.RightButton
      
      property int prevX: -1
      property int prevY: -1
      
      onPositionChanged: {
        if ( pressedButtons & Qt.RightButton) {
          
          var dx = 0;
          var dy = 0;
          if( prevX > -1 ) {
            dx = 0.01 * (mouse.x - prevX)
          }
          if( prevY > -1 ) {
            dy = 0.01 * (mouse.y - prevY)
          }
          cam1.spaceship_rotate( dx, dy )
          prevX = mouse.x
          prevY = mouse.y
        }
      }
      
      onReleased: { prevX = -1; prevY = -1; }
    }    
  }
  
  // Executed when everything is loaded
  Component.onCompleted: {
    cam1.znear = 1
    cam1.zfar  = 15000
    cam1.position = Qt.vector3d( 0, 0, 10000 )
    cam1.forward  = Qt.vector3d( 0, 0, -1 )
    cam1.up       = Qt.vector3d( 0, 1,  0 )
    cam1.right    = Qt.vector3d( 1, 0,  0 )
    cam1.translation_speed = 1.5
  }
  
  Text {
    id: fps
    anchors.top: parent.top
    anchors.right: parent.right
  }  
  
  Timer {
    interval: 2000; running: true; repeat: true
    onTriggered: fps.text = "FPS: " + glwindow.get_fps().toFixed(2);
  }
}