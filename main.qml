import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import ScenarioListModel 1.0
import ResourcesFetcher 1.0
import QtQuick.Dialogs 1.2
import "qrc:/ui"
Item {

    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true
    //width: 2560
    //height: 1600

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
    }

    Loader{
        id:scriptLoader
        property bool valid: item !== null
        anchors.fill: parent
        onLoaded: {
            intromenu.visible=false;
            intromenu.enabled=false;
            item.structureUrl=gridview.model.get(gridview.currentIndex,"ScenarioStructure");
            item.structure3DAsset=gridview.model.get(gridview.currentIndex,"Scenario3DAsset");
            item.structureTagConfig=gridview.model.get(gridview.currentIndex,"ScenarioTagConfiguration");
        }
    }

    Connections {
        ignoreUnknownSignals: true
        target: scriptLoader.valid ? scriptLoader.item : null
        onPageExit: { intromenu.visible=true;intromenu.enabled=true;scriptLoader.source=""}
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

            Component{
                id:modelDelegate
                Rectangle{
                    color: "#55FFFF99"
                    width: gridRect.height; height:gridRect.height
                    ColumnLayout{
                        Item{
                            //Layout.alignment: Qt.AlignCenter
                            Layout.preferredHeight:  gridRect.height-2*caption.font.pixelSize;
                            Layout.preferredWidth: gridRect.height;
                            Image {
                                anchors.fill: parent
                                anchors.margins: 10
                                fillMode: Image.PreserveAspectFit
                                source: decoration;
                            }
                        }
                        Item{
                            //Layout.alignment: Qt.AlignCenter
                            Layout.preferredHeight:caption.font.pixelSize
                            Layout.minimumWidth: gridRect.height
                            Text { id:caption ; text: display; font.pixelSize: pt2px(11);
                                anchors.centerIn: parent
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: gridview.currentIndex = index
                        }
                    }
                }
            }

            ListView{
                orientation: Qt.Horizontal
                id: gridview
                clip:true
                spacing: 10
                anchors.fill: parent
                model: ScenarioListModel{ }
                highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
                delegate:modelDelegate

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
            Button {
                text: "Button"
                onClicked: {
                    if(gridview.currentIndex !=-1){
                        scriptLoader.source="qrc:/scripts/Scripts/DefaultScript.qml"
                    }
                }
                anchors.right: parent.right
                anchors.rightMargin: 10
            }

            Image{
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

            Image{
                width: 100
                height: 100
                source: "qrc:/icons/Icons/material_design.png"
                anchors.bottom:  parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.rightMargin: 10
                MouseArea{
                    anchors.fill: parent
                    onClicked: materialDesigner.visible=true
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
