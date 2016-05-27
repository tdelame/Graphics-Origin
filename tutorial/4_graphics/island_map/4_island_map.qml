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
    
    // Executed when the component is loaded.
    Component.onCompleted: {
      // Test if we can build a SceneWindow, that is the component described in SceneWindow.qml
      var component = Qt.createComponent("SceneWindow.qml");
      if( component.status != Component.Ready ) {
  			if( component.status == Component.Error )
          console.debug("Error:"+ component.errorString() );
        return; // or maybe throw
      }

	  // Apparently, we can create a SceneWindow, so we create one:
	  // it will be a child of "main" and we define its position to (50,10)      
      var w = component.createObject(main,{"x":50,"y":10});
      
      w.cam1.znear = 0.0001
      w.cam1.zfar  = 15.0
      w.cam1.position = Qt.vector3d( 0, 0, 10 )
      w.cam1.forward  = Qt.vector3d( 0, 0, -1 )
      w.cam1.up       = Qt.vector3d( 0, 1,  0 )
      w.cam1.right    = Qt.vector3d( 1, 0,  0 )
      w.state = "FULLSCREEN"
    }
}