import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
Rectangle {
    id:root
    property var current_item
    state: "designer"
    states: [
        State {
            name: "beam"
            PropertyChanges {
                target: loader
                source:"qrc:/ui/UI/InfoBoxBeam.qml"
            }
        },
        State {
            name: "joint"
            PropertyChanges {
                target: loader
                source:"qrc:/ui/UI/InfoBoxJoint.qml"
            }
        },
        State {
            name: "designer"
            PropertyChanges {
                target: loader
                source:"qrc:/ui/UI/InfoBoxDesigner.qml"
            }
        }
    ]

    onCurrent_itemChanged: {
        if(applicationRoot.currentViewFilter=='DESIGNER'){
            state="designer"
            loader.item.current_item=Qt.binding(function(){return current_item})
        }
        else if(current_item != null){
            if(applicationRoot.currentViewFilter=='BEAM' && current_item.type==="beam"){
                state="beam";
            }
            else if(applicationRoot.currentViewFilter=='JOINT' && current_item.type==="joint"){
                state="joint"
            }
         }
    }

    color:"transparent"
    radius:5
    visible:applicationRoot.currentViewFilter=='DESIGNER'?
                true : current_item!=null

    Rectangle{
        //close button
        visible: applicationRoot.currentViewFilter!='DESIGNER'
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
        asynchronous: true
        onLoaded: item.current_item=Qt.binding(function(){return current_item})
    }

}

