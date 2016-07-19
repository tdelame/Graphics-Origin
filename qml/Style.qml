pragma Singleton
import QtQuick 2.3


// This is a nice way to define properties in QML. You have then just one file 
// where all the style is defined and can replace its content with another one
// when needed. Then, in other qml files, you just have to type 
// Style.main_window.initial_width for example to use the value defined in this
// style file.
QtObject {
  property QtObject main_window: QtObject {
    property int initial_width: 800
    property int initial_height: 600
    property color background: "#dbeafa"
  }

  property QtObject scene_window: QtObject {
    property int initial_width: 400
    property int initial_height: 412
    property int border_width: 2
    property color border_color: "#155093"
    property int radius: 5
    property string initial_state: "FULLSCREEN"

    property QtObject decoration: QtObject {
      property int height: 12
      property color color: "#3284e2"
    }

    property QtObject toolbar: QtObject {
      property int spacing: 6
    }
  
  }

  property QtObject z: QtObject {
    property int app_menu: 500
    property int scene_window: 600
    property int toolbar: 400
  }
}

