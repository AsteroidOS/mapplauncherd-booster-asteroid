import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import Qt.labs.folderlistmodel
import org.asteroid.controls
import org.asteroid.utils
import Nemo.Configuration
import QtSensors
import QtMultimedia
import Qt5Compat.GraphicalEffects

Application {
    BorderGestureArea { }
    HandWritingKeyboard { }
    Indicator { }
    LayerStack { }
    PageDot { }

    Label    { text: "A" }
    Marquee  { text: "A" }
    StatusPage { text: "x"; icon: "ios-add" }
    IconButton { iconName: "ios-add" }
    Switch     { checked: true }
    TextField  { text: "x" }
    Spinner         { }
    CircularSpinner { }

    Item            { visible: false }
    Rectangle       { visible: false }
    MouseArea       { visible: false }
    Timer           { }
    Component       { id: __preload_component_; Item {} }
    Repeater        { model: 0 }
    ListView        { visible: false; model: 0; delegate: Item {} }
    Image           { visible: false }
    ConfigurationValue { key: "/desktop/asteroid/booster/preload"; defaultValue: 0 }

    RowLayout    { visible: false; Item { Layout.fillWidth:  true } }
    ColumnLayout { visible: false; Item { Layout.fillHeight: true } }
    GridLayout   { visible: false; columns: 1; Item { } }
    StackLayout  { visible: false; Item { } }

    FolderListModel { folder: "file:///tmp"; showFiles: false; rootFolder: "file:///tmp" }

    Image {
        visible: false
        asynchronous: false
        sourceSize: Qt.size(1, 1)
        source: "data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='1' height='1'/>"
    }

    InputPanel { visible: false }
}
