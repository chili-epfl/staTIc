import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
Rectangle {
    id:root
    property var current_item:0;
    property alias lateral_visibility:hideButton.state
    onLateral_visibilityChanged: logger.log("infobox_change_lateral_visibility",{"visibility":lateral_visibility})
    property alias custom_loader: custom_loader
    property alias loader_beam: loader_beam
    property alias loader_designer: loader_designer
    property alias loader_joint: loader_joint
    MouseArea{
        anchors.fill: parent
    }

    color:"transparent"
    radius:5
    visible: settings.show_info_box
    onVisibleChanged: {
            logger.log("infobox_change_visibility",{"visible":visible,"visible_loader":settings.visible_loader})
    }

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
            onPressed: hideButton.state==="Visible" ? hideButton.state="Hidden": hideButton.state="Visible"
        }
    }

    Loader{
        id:custom_loader
        anchors.fill:parent
        asynchronous: false
        onLoaded: if(item.hasOwnProperty('current_item')) item.current_item=Qt.binding(function(){return current_item})
        visible:settings.visible_loader=="CUSTOM"
        onVisibleChanged: {
            if(item && item.current_item)
                logger.log("infobox_custom_loader_visibility",{"visible":visible,"item": item.current_item.objectName})
            else
                logger.log("infobox_custom_loader_visibility",{"visible":visible,"item": ""})
        }
    }
    Loader{
        id:loader_designer
        anchors.fill:parent
        visible: settings.visible_loader=="DESIGNER"
        asynchronous: true
        onLoaded: item.current_item=Qt.binding(function(){return current_item})
        source:"qrc:/ui/UI/InfoBoxDesigner.qml"
        onVisibleChanged: {
            if(item && item.current_item)
                logger.log("infobox_designer_loader_visibility",{"visible":visible,"item": item.current_item.objectName})
            else
                logger.log("infobox_designer_loader_visibility",{"visible":visible,"item": ""})
        }
    }
    Loader{
        id:loader_beam
        anchors.fill:parent
        visible: settings.visible_loader=="BEAM"
        asynchronous: true
        source:"qrc:/ui/UI/InfoBoxBeam.qml"
        onLoaded: item.current_item=Qt.binding(function(){return current_item})
        onVisibleChanged: {
            if(item && item.current_item)
                logger.log("infobox_beam_loader_visibility",{"visible":visible,"item": item.current_item.objectName})
            else
                logger.log("infobox_beam_loader_visibility",{"visible":visible,"item": ""})
        }
    }
    Loader{
        id:loader_joint
        visible: settings.visible_loader=="JOINT"
        anchors.fill:parent
        asynchronous: true
        onLoaded: item.current_item=Qt.binding(function(){return current_item})
        source:"qrc:/ui/UI/InfoBoxJoint.qml"
        onVisibleChanged: {
            if(item && item.current_item)
                logger.log("infobox_joint_loader_visibility",{"visible":visible,"item": item.current_item.objectName})
            else
                logger.log("infobox_joint_loader_visibility",{"visible":visible,"item": ""})
        }

    }

}

