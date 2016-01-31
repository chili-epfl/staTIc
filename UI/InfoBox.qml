import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Rectangle {
    id:root
    property var current_item

    states: [
        State {
            name: "beam"
            PropertyChanges {
                target: loader
                source:"qrc:/ui/UI/InfoBoxBeam.qml"
            }
            PropertyChanges{
                target: root
                visible:true
            }
        },
        State {
            name: "joint"
            PropertyChanges {
                target: loader
                source:"qrc:/ui/UI/InfoBoxJoint.qml"
            }
            PropertyChanges{
                target: root
                visible:true
            }
        },
        State {
            name: "null"
            PropertyChanges {
                target: loader
                source:""
            }
            PropertyChanges{
                target: root
                visible:false
            }
        }
    ]

    onCurrent_itemChanged: {
         if(current_item != null){
            if(current_item.type==="beam"){
                state="beam";
            }
            else if(current_item.type==="joint"){
                state="joint"
            }
         }
         else{
             state="null"
         }
    }

    color:"transparent"
    radius:5
    visible:false

    Rectangle{
        //close button
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
            onClicked: { root.current_item=null;}
        }
    }

    Loader{
        id:loader
        anchors.fill:parent
        onLoaded: item.current_item=current_item
    }

}

