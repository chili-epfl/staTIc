import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import ScenarioListModel 1.0
import ResourcesFetcher 1.0
import QtQuick.Dialogs 1.2
import "qrc:/ui"
//import QtSensors 5.3

Item {
    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true
    //width: 2560
    //height: 1600
//    Gyroscope{
//        id:gyro
//        active: true
//    }
    property bool use_custom_board:false

    Image{
        anchors.fill: parent
        fillMode: Image.Tile
        id:background
        source: "qrc:/images/Images/woodbackground.png"
    }

    ResourcesFetcher{
        id:resourcesFetcher
        hostUrl: "http://localhost:3003/api/carpenterData"
        username: "lollo"
        onLoadingChanged: if(!loading) scenarioListModel.reloadResources();
    }
    ScenarioListModel{
        id:scenarioListModel
        onDataChanged:
        {
            gridview.model="null"
            gridview.model=scenarioListModel
        }
    }

    Loader{
        id:scriptLoader
        property bool valid: item !== null
        asynchronous: false
        anchors.fill: parent
        onLoaded: {
            intromenu.visible=false;
            intromenu.enabled=false;
            item.structureUrl=gridview.model.get(gridview.currentIndex,"ScenarioStructure");
            item.structure3DAsset=gridview.model.get(gridview.currentIndex,"Scenario3DAsset");
        }
    }

    Connections {
        ignoreUnknownSignals: true
        target: scriptLoader.valid ? scriptLoader.item : null
        onPageExit: {
            logger.close_logger();
            intromenu.visible=true;
            intromenu.enabled=true;
            scriptLoader.source=""
        }
    }

    /*Item conteining all the item for the intro menu.*/
    ColumnLayout{
        anchors.fill: parent
        anchors.margins:10
        id:intromenu
        spacing: 5

        /*Banner*/
        Item{
            id:banner
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.2
            Layout.alignment: Qt.AlignCenter
            Image{
                anchors.centerIn: parent
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source:"qrc:/images/Images/mainbanner.png"
            }
        }

        /*Scenario List*/
        Rectangle {
            id:gridRect
            color:"transparent"
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.6
            Layout.alignment: Qt.AlignCenter


            ListView{
                orientation: Qt.Horizontal
                highlightMoveDuration:400
                id: gridview
                clip:true
                spacing: 10
                anchors.fill: parent
                model: scenarioListModel
                highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
                delegate: Rectangle{
                    color: "#55FFFF99"
                    width: gridRect.height; height:gridRect.height
                    ColumnLayout{
                        anchors.fill: parent
                        Item{
                            id:scenarioListView
                            //Layout.alignment: Qt.AlignCenter
                            Layout.preferredHeight:  0.8*parent.height;
                            Layout.preferredWidth: parent.height;
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                fillMode: Image.PreserveAspectFit
                                source: decoration;
                            }
                        }
                        Item{
                            //Layout.alignment: Qt.AlignCenter
                            Layout.preferredHeight:  0.2*parent.height;
                            Layout.preferredWidth: parent.height;
                            Text {
                                id:caption;
                                text: display;
                                anchors.fill: parent
                                fontSizeMode: Text.Fit
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: gridview.currentIndex = index
                        }
                    }
                }

                flickableChildren: MouseArea {
                    anchors.fill: parent
                    onClicked: gridview.currentIndex = -1
                }
            }
        }

        /*Button*/
        Item{
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.1

            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.1

            Image{
                id:synch_img
                width: 100
                height: 100
                source: synch_mouse_area.pressed ? "qrc:/icons/Icons/synch_pressed.png" : "qrc:/icons/Icons/synch_ON.png"
                anchors.bottom:  parent.bottom
                anchors.rightMargin: 10

                MouseArea{
                    id:synch_mouse_area
                    anchors.fill: parent
                    onClicked: synch_dialog_box.visible=true
                }
            }
            Rectangle{
                anchors.left: synch_img.right
                anchors.bottom:  parent.bottom
                id:custom_board_enable_rect
                width: 100
                height: 100
                color: use_custom_board ? "green" : "red"
                Text {
                    anchors.fill: parent
                    fontSizeMode: Text.Fit
                    wrapMode: Text.Wrap
                    text: "Use Custom Board"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: use_custom_board=!use_custom_board
                }
            }

            Image{
                width: 100
                height: 100
                source: "qrc:/icons/Icons/material_design.png"
                anchors.right: parent.right
                anchors.bottom:  parent.bottom
                anchors.rightMargin: 10
                MouseArea{
                    anchors.fill: parent
                    onClicked: if(gridview.currentIndex !=-1){
//                                   scriptLoader.source="qrc:/scripts/Scripts/CableExercise.qml"
                                     scriptLoader.source="qrc:/scripts/Scripts/DefaultScript.qml"
//                                   scriptLoader.source="qrc:/scripts/Scripts/SupportExercise.qml"
                                     logger.restart_logger();
                               }
                }
            }
        }
    }
    SynchDialogBox{
        id:synch_dialog_box
        resourcesFetcher: resourcesFetcher
    }
    MaterialDesigner{
        id:materialDesigner
    }


}
