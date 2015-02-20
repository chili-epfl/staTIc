import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.2

ApplicationWindow {

    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight

    Item {
        id: item1
        anchors.fill: parent

        Button {
            id: button1
            width: 125
            height: 53
            text: qsTr("Draw")
            anchors.bottom: button2.top
            anchors.bottomMargin: 22
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: button2
            width: button1.width
            height: button1.height
            text: qsTr("Print")
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: button3
            width: button1.width
            height: button1.height
            text: qsTr("Explore")
            anchors.top: button2.bottom
            anchors.topMargin: 22
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: loader.source="exploreView.qml"
        }
    }
    Loader{
        id:loader
        anchors.fill: parent
    }




}
