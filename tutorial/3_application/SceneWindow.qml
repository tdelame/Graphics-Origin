import QtQuick 2.3
import QtQuick.Controls 1.4
import GraphicsOrigin 1.0
import "." //QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
  id: main
  width: 800
  height:800
  visible: true

  GLCamera {
	  id: cam1
	  ratio: glwindow.height > 0 ? glwindow.width / glwindow.height : 1
	  Component.onCompleted: {
	  	console.log("ratio is set to " + ratio )
	  }
	}
  
  GLWindow {
  	id: glwindow
    width: parent.width - parent.border.width
		height: parent.height - parent.border.width
		anchors.centerIn: parent
		camera: cam1
  }
}