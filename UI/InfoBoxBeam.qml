import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Scene3D 2.0
import QtQuick.Layouts 1.1
import "qrc:/ui/UI"
Rectangle {
    id:root
    property var current_item
    color: "transparent"
    onCurrent_itemChanged: {
        if(current_item != 0 && current_item.segments){
            title.text="Beam "+ current_item.objectName;
            infoboxscene3d.extreme1_name=current_item.extreme1_name;
            infoboxscene3d.extreme2_name=current_item.extreme2_name;
            if(current_item.extreme1.y>current_item.extreme2.y)
                infoboxscene3d.upsideDown=true
            else
                infoboxscene3d.upsideDown=false
            logger.log("infobox_beam_item_changed",{"item":current_item.objectName})
        }
    }

    ColumnLayout{
        spacing: 10

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
                    exagerate:{
                        switch( exagerateSlider.value){
                        case 1 :
                            return 1
                        case 2 :
                            return 2
                        case 3 :
                            return 5
                        case 4 :
                            return 10
                        case 5 :
                            return 25
                        case 6 :
                            return 50
                        case 7 :
                            return 100
                        case 8 :
                            return 200
                        case 9 :
                            return 400
                        default:
                            return 1
                        }
                    }
                    Binding on beamSize {
                        when: current_item!=0 && current_item.type=="beam"
                        value:current_item.beamSize
                    }

                    Binding on segments {
                        when: current_item!=0 && current_item.type=="beam"
                        value:current_item.segments
                    }
                    Binding on beamLength {
                        when: current_item!=0 && current_item.type=="beam"
                        value:current_item.length
                    }
                }
            }


            Rectangle{
                id:material_info_box
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 30
                width: parent.width/4
                height: 4*width/3
                color:"#80000000"
                border.color: "black"
                radius: 3
                border.width: 1
                ColumnLayout{
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing:5
                    Text{
                        Layout.preferredWidth: material_info_box.width
                        Layout.preferredHeight: 0.3*material_info_box.height
                        id: materialInfo
                        color:"#F8F8F8"
                        text: "<b>Material:</b> "+materialsManager.get(current_item.materialID,"name");
                        fontSizeMode: Text.Fit;
                        wrapMode: Text.WordWrap
                        minimumPixelSize: 8;
                        font.pixelSize: 20
                    }
                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight:  true
                        Image {
                            id:beam_section_image
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/images/Images/beam_section.png"
                            rotation:current_item!=0 && current_item.type=="beam" && current_item.realBeamSize.width > current_item.realBeamSize.height ? 90:0

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
                                text: current_item!=0 && current_item.type=="beam" ? qsTr("w: ")+current_item.realBeamSize.width+"mm" : ""
                                anchors.top: parent.top
                                horizontalAlignment:Text.AlignHCenter
                                fontSizeMode: Text.Fit;
                                minimumPixelSize: 10;
                                font.pixelSize: 13
                            }
                            Text{
                                width: parent.height
                                color:"black"
                                text:current_item!=0 && current_item.type=="beam"? qsTr("h: ")+current_item.realBeamSize.height+"mm" :""
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
                    onPressed: {
                        infoboxscene3d.zoom=1
                        logger.log("infobox_beam_zoom",{"direction":"In"})
                    }
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
                    onPressed: {
                        infoboxscene3d.zoom=-1
                        logger.log("infobox_beam_zoom",{"direction":"Out"})
                    }
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
                    onClicked: {
                        infoboxscene3d.resize=true
                        logger.log("infobox_beam_zoom",{"direction":"resize"})
                    }
                }
            }

            Switch {
                id:controlSwitch
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.horizontalCenter:  parent.horizontalCenter
                height: 35
                width:2*height
                style: CustomSwitchStyle{

                }
                checked: false
                onCheckedChanged: {
                    infoboxscene3d.panMode=checked;
                    logger.log("infobox_beam_navigation",{"pan_mode":checked})
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
                    text: "Exagerate: "+infoboxscene3d.exagerate+"x"
                    color: "white"
                }
                Slider{
                    id:exagerateSlider
                    anchors.top: exagerateLabel.bottom
                    width: parent.width
                    value:1
                    stepSize: 1
                    tickmarksEnabled: true
                    minimumValue: 1
                    maximumValue: 9
                    onValueChanged:
                        logger.log("infobox_beam_exagerate",{"value":value})
                }
            }
        }
    }
}

