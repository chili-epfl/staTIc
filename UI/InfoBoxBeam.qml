import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Scene3D 2.0
import QtQuick.Layouts 1.1
Rectangle {
    id:root
    property var current_item
    color: "transparent"
    onCurrent_itemChanged: {
        if(current_item != null && current_item.segments){
            title.text="Beam "+ current_item.objectName;
            infoboxscene3d.extreme1_name=current_item.extreme1_name;
            infoboxscene3d.extreme2_name=current_item.extreme2_name;
            //            materialInfo.val=Qt.binding(function(){return current_item.material;})
            //            sizeInfo.val=Qt.binding(function(){return current_item.size;})
            //            statusInfo.val=Qt.binding(function(){return current_item.status;})
            //            weightInfo.val=Qt.binding(function(){return current_item.weight;})
            //            priceInfo.val=Qt.binding(function(){return current_item.price;})
            infoboxscene3d.segments=Qt.binding(function(){
                return current_item.segments;})
            infoboxscene3d.beamSize=Qt.binding(function(){
                return current_item.beamSize;})
            infoboxscene3d.beamLength=Qt.binding(function(){
                return current_item.length;})
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
                InfoBoxScene3DBeam {
                    id:infoboxscene3d
                    exagerate:exagerateSlider.value
                }
            }


            Rectangle{
                id:material_info_box
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 30
                width: parent.width/2
                height: 3*width/4
                color:"#80000000"
                border.color: "black"
                radius: 3
                border.width: 1
                ColumnLayout{
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing:5
                    Text {
                        Layout.fillWidth: true
                        id: materialInfo
                        color:"#F8F8F8"
                        text: "<b>Material:</b> "+materialsManager.get(current_item.materialID,"name");
                        fontSizeMode: Text.Fit;
                        minimumPixelSize: 10;
                        font.pixelSize: 20
                        textFormat: Text.RichText
                    }
                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight:  true
                        Image {
                            id:beam_section_image
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/images/Images/beam_section.png"
                            rotation: current_item.realBeamSize.width > current_item.realBeamSize.height ? 90:0
                        }
                        Item {
                            width: beam_section_image.rotation==0 ? beam_section_image.paintedWidth :beam_section_image.paintedHeight
                            height:beam_section_image.rotation==0 ? beam_section_image.paintedHeight:beam_section_image.paintedWidth
                            anchors.horizontalCenter: beam_section_image.horizontalCenter
                            anchors.verticalCenter: beam_section_image.verticalCenter
                            Text{
                                width: parent.width
                                anchors.topMargin: 1
                                color:"black"
                                text: qsTr("w: ")+current_item.realBeamSize.width+"mm"
                                anchors.top: parent.top
                                horizontalAlignment:Text.AlignHCenter
                                fontSizeMode: Text.Fit;
                                minimumPixelSize: 10;
                                font.pixelSize: 13
                            }
                            Text{
                                width: parent.height
                                color:"black"
                                text: qsTr("h: ")+current_item.realBeamSize.height+"mm"
                                anchors.top: parent.bottom
                                anchors.left: parent.left
                                rotation: -90
                                horizontalAlignment:Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                fontSizeMode: Text.Fit;
                                minimumPixelSize: 10;
                                font.pixelSize: 13
                                transformOrigin:Item.TopLeft
                            }
                        }
                    }
                    //                    Text {
                    //                        id: sizeInfo
                    //                        property string val
                    //                        color:"#F8F8F8"
                    //                        text: qsTr("Size:")+val
                    //                    }
                    //                    Text {
                    //                        id: weightInfo
                    //                        property string val
                    //                        color:"#F8F8F8"
                    //                        text: qsTr("Weight:")+val
                    //                    }
                    //                    Text {
                    //                        id: priceInfo
                    //                        property string val
                    //                        color:"#F8F8F8"
                    //                        text: qsTr("Price:")+val
                    //                    }
                    //                    Text {
                    //                        id: statusInfo
                    //                        property string val
                    //                        color:"#F8F8F8"
                    //                        text: qsTr("Status:")+val
                    //                    }
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


            //----------Navigation UI-----------------
            Image {
                id: plusButton
                source: "qrc:/icons/Icons/plus.png"
                width: 75
                height: 75
                anchors.margins: 30
                anchors.bottom: minusButton.top
                anchors.left: parent.left
                Binding on source {
                    when: mouseArea_plus.pressed
                    value:"qrc:/icons/Icons/plus_pressed.png"
                }
                Binding on source {
                    when: !mouseArea_plus.pressed
                    value:"qrc:/icons/Icons/plus.png"
                }
                MouseArea{
                    id:mouseArea_plus
                    anchors.fill: parent
                    onPressed: infoboxscene3d.zoom=1
                    onReleased: infoboxscene3d.zoom=0
                    onCanceled: infoboxscene3d.zoom=0
                }
            }
            Image {
                id: minusButton
                source: "qrc:/icons/Icons/minus.png"
                width: 75
                height: 75
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                Binding on source {
                    when: mouseArea_minus.pressed
                    value:"qrc:/icons/Icons/minus_pressed.png"
                }
                Binding on source {
                    when: !mouseArea_minus.pressed
                    value:"qrc:/icons/Icons/minus.png"
                }
                MouseArea{
                    id:mouseArea_minus
                    anchors.fill: parent
                    onPressed: infoboxscene3d.zoom=-1
                    onReleased: infoboxscene3d.zoom=0
                    onCanceled: infoboxscene3d.zoom=0
                }
            }
            Image {
                id: resizeButton
                source: "qrc:/icons/Icons/resize.png"
                width: 75
                height: 75
                anchors.margins: 30
                anchors.top: parent.top
                anchors.right: parent.right
                Binding on source {
                    when: mouseArea_resize.pressed
                    value:"qrc:/icons/Icons/resize_pressed.png"
                }
                Binding on source {
                    when: !mouseArea_resize.pressed
                    value:"qrc:/icons/Icons/resize.png"
                }
                MouseArea{
                    id:mouseArea_resize
                    anchors.fill: parent
                    onClicked: infoboxscene3d.resize=true
                }
            }

            Switch {
                id:controlSwitch
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.horizontalCenter:  parent.horizontalCenter
                height: 35
                checked: true
                onCheckedChanged: {
                    infoboxscene3d.panMode=checked;
                }

            }
            Image {
                id: moveIcon
                source: "qrc:/icons/Icons/move.png"
                width: 35
                height: 35
                anchors.rightMargin:  10
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.right: controlSwitch.left
            }
            Image {
                id: panIcon
                source: "qrc:/icons/Icons/pan.png"
                width: 35
                height: 35
                anchors.margins: 30
                anchors.leftMargin: 10
                anchors.bottom: parent.bottom
                anchors.left: controlSwitch.right
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
                    maximumValue: 400

                }
            }

        }
    }
}

