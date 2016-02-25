import QtQuick 2.5
import QtQuick.Controls 1.4
import GraphicsOrigin 1.0
import "." //QTBUG-34418, singletons require explicit import to load qmldir file

Item {
  id: scene_window
  width: Style.scene_window.initial_width
  height: Style.scene_window.initial_width
  z: Style.z.scene_window
  
  /****************************************************************************
  * State Management
  *****************************************************************************/
  // variable for the state changes
  state: Style.scene_window.initial_state
  property variant return_size: Qt.point( width, height ) 
  property variant return_position: Qt.point( x, y )
  // state definition
  states: [
    // normal state: when the window is not maximalized
    State {
      name: "NORMAL"
      StateChangeScript {
        name: "normal_script"
        script: {
         	glwindow.resume();
 	        width = return_size.x;
          height= return_size.y;  
          x = return_position.x;
          y = return_position.y;
          content.visible = true;
        }
      }
    },
    State {
      name: "FULLSCREEN"
      StateChangeScript {
        name: "fullscreen_script"
        script: {
          return_size = Qt.point( width, height );
          return_position = Qt.point( x, y );
          x = 0;
          y = 0;
          height = main.height;
          width = main.width;
          glwindow.resume();
        }
      }
    }, 
    State {
      name: "MINIMIZED"
      PropertyChanges {
      	target: content
        visible: false
      }
      StateChangeScript {
        name: "minimized_script"
        script: {
          glwindow.pause();
        }
      }
    }
  ]
  
 /*****************************************************************************
  * Decoration: to control the window position/size
  *****************************************************************************/
  Rectangle {
    id: decoration
    anchors.top: scene_window.top
    anchors.left: scene_window.left
    anchors.right: scene_window.right
    height: Style.scene_window.decoration.height
    color: Style.scene_window.decoration.color
    
    MouseArea {
      id: bar
      anchors.top: parent.top
      anchors.left: parent.left
      anchors.bottom: parent.bottom
      width: parent.width - 3 * parent.height
      property variant clickPos: "1,1"

      onPressed: {
        clickPos = Qt.point(mouse.x, mouse.y)
      }
      
      onDoubleClicked: {
        if( scene_window.state == "NORMAL" ) {
          scene_window.state = "FULLSCREEN";
        } else {
          scene_window.state = "NORMAL";
        }
      }

      onPositionChanged: {
        scene_window.x = scene_window.x + mouse.x - clickPos.x;
        scene_window.y = scene_window.y + mouse.y - clickPos.y;
        if( scene_window.x < 0 ) scene_window.x = 0
        if( scene_window.y < 0 ) scene_window.y = 0
        if( (scene_window.x + scene_window.width) > main.width ) scene_window.x = main.width - scene_window.width
        if( (scene_window.y + scene_window.height) > main.height) scene_window.y = main.height - scene_window.height
      }
    }

    Rectangle {
      id: destroy
      anchors.right: parent.right
      
      width: parent.height
      height: parent.height
      border.width: 1
      border.color: "black"
      color: "red"

      MouseArea {
        anchors.fill: parent
        onPressed: {
          scene_window.destroy();
        }
      }
    }

    Rectangle {
      id: maximize

      anchors.right: destroy.left
      width: parent.height
      height: parent.height
      color:"green"
      border.width: 1
      border.color: "black"

      MouseArea {
        anchors.fill: parent
        onPressed: {
          if( scene_window.state == "MINIMIZED" ) {
          	scene_window.state = "NORMAL"
          } else {
          	scene_window.state = "FULLSCREEN"
          }
        }
      }
    }

    Rectangle {
      id: minimize

      anchors.right: maximize.left
      width:parent.height
      height: parent.height
      color: "blue"
      border.width: 1
      border.color: "black"

      MouseArea {
        anchors.fill: parent
        onPressed: {
          if( scene_window.state == "FULLSCREEN" ) {
            scene_window.state = "NORMAL"
          } else {
            scene_window.state = "MINIMIZED"
          }
        }
      }
    }
  }

  // put here a menu bar
  
  // put here the toobar
  
  // put here the status bar
  
  
  /****************************************************************************
   * Content: GL 3D Scene
   ****************************************************************************/
  Rectangle {
	  id: content
    anchors.bottom: parent.bottom
    width: scene_window.width
	  height: scene_window.height - decoration.height
	  anchors.left: scene_window.left
	  y: scene_window.y + decoration.height
    border.width: Style.scene_window.border_width
    border.color: Style.scene_window.border_color
    
    GLCamera {
      id: cam1
      ratio: glwindow.height > 0 ? glwindow.width / glwindow.height : 1
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
    
    GLWindow {
  	  id: glwindow
      width: parent.width - parent.border.width
      height: parent.height - parent.border.width
      anchors.centerIn: parent
      camera: cam1
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
  	    }
      }      
      
     Text {
      id: fps
      anchors.top: parent.top;
      anchors.right: parent.right;
     }
    
     Timer {
       interval: 2000; running: true; repeat: true
       onTriggered: fps.text = "FPS: " + glwindow.get_fps().toFixed(2);
     }
      
  	}
  }
}