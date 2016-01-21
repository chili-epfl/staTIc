import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Scene3D 2.0
import QtQuick.Layouts 1.1
Rectangle {
    id:root
    property alias segments: infoboxscene3d.segments
    property alias syncPose: infoboxscene3d.syncPose

    property var current_item
    onCurrent_itemChanged: {
         if(current_item){
            segments=Qt.binding(function(){return current_item.segments;})
            title.text="Beam "+ current_item.objectName;
//            materialInfo.val=Qt.binding(function(){return current_item.material;})
//            sizeInfo.val=Qt.binding(function(){return current_item.size;})
//            statusInfo.val=Qt.binding(function(){return current_item.status;})
//            weightInfo.val=Qt.binding(function(){return current_item.weight;})
//            priceInfo.val=Qt.binding(function(){return current_item.price;})
            infoboxscene3d.deformableMeshLength=Qt.binding(function(){return current_item.length/10;})
            infoboxscene3d.deformableMeshSize=Qt.binding(function(){return current_item.beamSize/10;})
            visible=true;

         }
    }


    color:"transparent"
    radius:5
    visible: false

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
            onClicked: {root.visible=false; current_item=null}
        }
    }
    ColumnLayout{
          spacing: 10
//        LinearGradient {
//            anchors.fill: parent
//            start: Qt.point(0, parent.height)
//            end: Qt.point(parent.width, 0)
//            gradient: Gradient {
//                GradientStop { position: 0.0; color: "transparent" }
//                GradientStop { position: 0.3; color: "#80ffb90f" }
//            }
//        }

        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.05*root.height
            Item{
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    anchors.fill: parent
                    id: title
                    text: qsTr("text")
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10; font.pixelSize: 72
                    color: "white"
                }
            }
        }
        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.70*root.height
            Scene3D {
                anchors.fill: parent
                anchors.margins: 10
                focus: true
                aspects: ["input","logic"]
                multisample: true
                InfoBoxScene3D {
                    id:infoboxscene3d
                }
            }
            Item{
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 30
                ColumnLayout{
                    spacing:2
                    Text {
                        id: materialInfo
                        property string val
                        color:"#F8F8F8"
                        text: qsTr("Material:")+val
                    }
                    Text {
                        id: sizeInfo
                        property string val
                        color:"#F8F8F8"
                        text: qsTr("Size:")+val
                    }
                    Text {
                        id: weightInfo
                        property string val
                        color:"#F8F8F8"
                        text: qsTr("Weight:")+val
                    }
                    Text {
                        id: priceInfo
                        property string val
                        color:"#F8F8F8"
                        text: qsTr("Price:")+val
                    }
                    Text {
                        id: statusInfo
                        property string val
                        color:"#F8F8F8"
                        text: qsTr("Status:")+val
                    }
                }
            }
            Image{
                id:animationIcon
                states: [
                    State {
                        name: "play"
                        PropertyChanges {
                            target: animationIcon
                            source: "qrc:/icons/Icons/pause.svg"
                        }
                        PropertyChanges {
                            target: infoboxscene3d
                            play: true
                        }
                    },
                    State {
                        name: "pause"
                        PropertyChanges {
                            target: animationIcon
                            source: "qrc:/icons/Icons/play.svg"
                        }
                        PropertyChanges {
                            target: infoboxscene3d
                            play: false
                        }
                    }
                ]

                state: "play"
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 30
                source: "qrc:/icons/Icons/play.svg"
                width: 75
                height: 75
                MouseArea{
                    anchors.fill: parent
                    onClicked: parent.state=="play"? parent.state="pause" : parent.state="play"
                }
            }
        }
        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: exagerateLabel.height+exagerateSlider.height+20
            Item{
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    id:exagerateLabel
                    text: qsTr("Exagerate")
                    color: "white"
                }
                Slider{
                    id:exagerateSlider
                    anchors.top: exagerateLabel.bottom
                    width: parent.width
                    value:1
                    minimumValue: 1
                    maximumValue: 200
                }
            }

        }



    }
}

