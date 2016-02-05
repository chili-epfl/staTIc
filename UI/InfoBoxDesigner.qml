import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
Rectangle {
    id:root
    property var current_item
    color: "transparent"
    TabView{
        anchors.fill: parent
        style:TabViewStyle {
            frame: Rectangle {
                color: "transparent"
                border.color: "#F0F0F0"
                border.width: 5
            }
        }
        Tab{
            title: "Catalog"
            Rectangle{
                anchors.fill:parent
                color:"#2f3439"
                radius: 1
            }
        }
        Tab{
            title:"Beam Materials"
            Rectangle{
                id:materialItem
                anchors.fill:parent
                anchors.margins: 10
                color: "transparent"
                Column{
                    anchors.fill: parent
                    spacing: 10
                    Rectangle{
                        width: parent.width
                        height: 2*parent.height/3
                        anchors.margins: 10
                        color:"transparent"
                        radius: 1
                        ListView{
                            id:materials_list
                            anchors.fill: parent;
                            anchors.margins: 10
                            model: materialsManager
                            clip: true
                            spacing: 10
                            delegate: Rectangle{
                                width: materialItem.width-20
                                height: (materialItem.width-20)/3
                                color:"#2f3439";
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: materials_list.currentIndex=index;
                                }
                                RowLayout{
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10
                                    Image {
                                        source:decoration
                                        Layout.preferredWidth:  height
                                        Layout.preferredHeight: parent.height
                                    }
                                    Rectangle{
                                        color:materials_list.currentIndex==index ?
                                                  "red" : "transparent";
                                        Layout.preferredWidth:  10
                                        Layout.preferredHeight: parent.height
                                    }
                                    Rectangle{
                                        color:"#2f3439"
                                        Layout.maximumWidth:  parent.width-parent.height-10
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: parent.height
                                        ColumnLayout{
                                            Layout.margins: 5
                                            Text {
                                                color: "white"
                                                text: display
                                            }
                                            Text {
                                                color: "white"
                                                text: "Price:"+materialsManager.get(index,"Price")
                                            }
                                            Text {
                                                color: "white"
                                                text: "Density:"+materialsManager.get(index,"Price")                                }
                                        }
                                        Image{
                                            visible: materials_list.currentIndex==index
                                            anchors.bottom: parent.bottom
                                            anchors.right: parent.right
                                            height: 50
                                            width: 50
                                            source: material_apply_button.pressed ? "qrc:/icons/Icons/apply_pressed.png":"qrc:/icons/Icons/apply.png"
                                            MouseArea{
                                                id:material_apply_button
                                                anchors.fill: parent
                                                //onClicked:
                                            }

                                        }
                                    }
                                }

                            }
                        }
                    }
                    /*Size modifier*/
                    Rectangle{
                        width: parent.width-10
                        height: parent.width/3
                        anchors.margins: 10
                        color:"#2f3439";
                        Text {
                            id:caption_size
                            color: "white"
                            text: "Size"
                            anchors.margins: 10
                            anchors.top:parent.top
                            anchors.left: parent.left

                        }
                        TextField{
                            id:w
                            anchors.verticalCenter: x.verticalCenter
                            anchors.right: x.left
                            anchors.rightMargin: 25
                            text:"1"
                            maximumLength: 5
                            validator: DoubleValidator {bottom: 1; top: 50000;}
                            width: parent.width/2-50

                        }
                        Text{
                            id:x
                            text:"X"
                            anchors.centerIn: parent

                        }

                        TextField{
                            id:h
                            maximumLength: 5
                            anchors.verticalCenter: x.verticalCenter
                            anchors.left: x.right
                            anchors.leftMargin: 25
                            width: parent.width/2-50
                            text:"1"
                            validator: DoubleValidator {bottom: 1; top: 50000;}
                        }
                        Text{
                            text: "W"
                            anchors.horizontalCenter: w.horizontalCenter
                            anchors.top: w.bottom
                            anchors.topMargin: 10
                        }
                        Text{
                            text: "H"
                            anchors.horizontalCenter: h.horizontalCenter
                            anchors.top: h.bottom
                            anchors.topMargin: 10
                        }
                        Image{
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            height: 50
                            width: 50
                            source: size_apply_button.pressed ? "qrc:/icons/Icons/apply_pressed.png":"qrc:/icons/Icons/apply.png"
                            MouseArea{
                                id:size_apply_button
                                anchors.fill: parent
                                onClicked: console.log("click")
                            }
                        }
                    }

                }


            }

        }
    }

}

