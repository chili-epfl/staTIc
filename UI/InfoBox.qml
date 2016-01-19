import QtQuick 2.0
import QtGraphicalEffects 1.0
 import QtQuick.Controls.Styles 1.4
 import QtQuick.Controls 1.2
Rectangle {
    id:root

//    state:"DISPLAYED"

//    states: [
//        State {
//            name: "DISPLAYED"
//            PropertyChanges { target: root; visible:true}
//        },
//        State {
//            name: "HIDDEN"
//            PropertyChanges { target: root; visible:false}

//        }
//    ]
    color: "transparent"
    radius:5
    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, parent.height)
        end: Qt.point(parent.width, 0)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 0.3; color: "#80ffb90f" }
        }
    }
    Rectangle{
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
            onClicked: root.visible=false;
        }
    }

}

