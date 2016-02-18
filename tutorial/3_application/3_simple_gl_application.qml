import QtQuick 2.5
import QtQuick.Controls 1.4
import GraphicsOrigin 1.0
import "." //QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    id: main
    width: Style.main_window.initial_width
    height: Style.main_window.initial_height
    color: Style.main_window.background
    visible: true
    
    Component.onCompleted: {
      var component = Qt.createComponent("SceneWindow.qml");
      if( component.status != Component.Ready ) {
  			if( component.status == Component.Error )
          console.debug("Error:"+ component.errorString() );
        return; // or maybe throw
      }
      var w = component.createObject(main,{"x":50,"y":10});
      Qt.createComponent("SceneWindow.qml");
    }
}