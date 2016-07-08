import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
Rectangle {
    id:root
    property var current_item:0;
    property alias lateral_visibility:hideButton.state
    property alias loader: loader

    color:"transparent"
    radius:5
    visible: settings.show_info_box

    Rectangle{
        //close button

        visible: false//applicationRoot.currentViewFilter!='DESIGNER'
        z:1
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: -width/2
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.5; color: "#700000" }
            GradientStop { position: 1.0; color: "black" }

        }
        border.color: "black"
        radius: width*0.5
        width: pt2px(30)
        height: width
        MouseArea{
            anchors.fill: parent
            onClicked: { root.current_item=0;}
        }
    }

    Rectangle{
        id:hideButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        state: "Visible"
        states:[
            State {
                name: "Hidden"
                PropertyChanges {
                    target: hideLabel
                    text: "Show"
                }
                AnchorChanges {
                    target: root
                    anchors.bottom: root.parent.bottom
                    anchors.left: root.parent.right
                    anchors.top: root.parent.top
                }
                PropertyChanges{
                    target:root
                    anchors.leftMargin: hideLabel.height*2
                }
            },
            State {
                name: "Visible"
                PropertyChanges {
                    target: hideLabel
                    text: "Hide"
                }
                AnchorChanges {
                    target: root
                    anchors.bottom: root.parent.bottom
                    anchors.right: root.parent.right
                    anchors.top: root.parent.top
                }
                PropertyChanges{
                    target:root
                    anchors.margins: 10
                }
            }
        ]
        transitions: Transition {
            AnchorAnimation { duration: 500 }
        }
        color:"#2f3439"
        height:  Math.max(parent.width * 0.25,hideLabel.width*1.2)
        width: parent.width * 0.05
        Label{
            id:hideLabel
            text: "Hide"
            font.pixelSize: parent.width*0.75
            color: "#F8F8F8"
            rotation: -90
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onClicked: hideButton.state==="Visible" ? hideButton.state="Hidden": hideButton.state="Visible"
        }
    }


    Rectangle{
        anchors.centerIn: parent
        width: busyIndicator.width*1.2
        height: width
        radius: width/2
        color: "#F8F8F8"
        visible: loader.status!=Loader.Ready
        BusyIndicator {
            id:busyIndicator
            anchors.centerIn: parent
            running: parent.visible
        }
    }

    Loader{
        id:loader
        anchors.fill:parent
        asynchronous: false
        onLoaded: item.current_item=Qt.binding(function(){return current_item})
        onSourceChanged: console.log(source)
    }

}

