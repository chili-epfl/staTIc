import QtQuick 2.0
import QtQuick.Controls.Styles 1.4

SwitchStyle {
    groove: Rectangle {
        implicitWidth: 100
        implicitHeight: 50
        radius: 25
        border.color: control.activeFocus ? "darkblue" : "gray"
        border.width: 1
        color: control.checked ? "#33B5E5" : "grey"
    }
    handle: Item{
        implicitHeight: 50
        implicitWidth: 50
        Rectangle{
            anchors.fill: parent
            anchors.margins: 2
            radius: height/2
        }
    }
}
