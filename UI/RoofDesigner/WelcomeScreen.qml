import QtQuick 2.7
import QtQuick.Controls 2.1
import Qt.labs.folderlistmodel 2.1
import QtQuick.Layouts 1.1
import JSONSketch 1.0
Page {
    id:welcome_page

    Rectangle{
        anchors.fill: parent
        color:"lightgrey"
    }
    RowLayout{
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: welcome_page.availableHeight/3
        anchors.bottomMargin: welcome_page.availableHeight/3
        anchors.fill:parent
        spacing: 10
        Rectangle{
            width: height*4/3
            height: welcome_page.availableHeight/3
            border.color: "black"
            border.width: 3
            Image{
                anchors.fill: parent
                anchors.margins: 10
                fillMode: Image.PreserveAspectFit
                source:"qrc:/ui/UI/RoofDesigner/icons/plus-1270001_1280.png"
            }
            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    loading_indicator.visible=true;
                    stack_view.push("qrc:/ui/UI/RoofDesigner/SketchScreen.qml");
                }
            }
        }
        ListView{
            id:sketch_list_view
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            orientation: ListView.Horizontal
            spacing: 10
            model: folder_list_model
            FolderListModel {
                id:folder_list_model
                showDirs: false
                folder: scenariosPath
                nameFilters: ["*.json"]
            }
            delegate:Rectangle{
                width: height*4/3
                height: welcome_page.availableHeight/3
                border.color: "black"
                border.width: 3
                JSONSketch{
                    id:json_sketch
                }
                Component.onCompleted: {
                    json_sketch.loadSketch(fileName,this)
                }
                MouseArea{
                    anchors.fill: parent
                    onDoubleClicked: {
                        loading_indicator.visible=true;
                        stack_view.push("qrc:/ui/UI/RoofDesigner/SketchScreen.qml",{"sketch_to_load":fileBaseName});
                        sketch_list_view.model=0
                    }
                }
            }
            BusyIndicator {
                id:loading_indicator
                visible:false
                Timer{
                    interval: 2000
                    repeat: false
                    running: loading_indicator.visible
                    onTriggered: {
                        loading_indicator.visible=false
                        sketch_list_view.model=folder_list_model
                    }
                }
                anchors.centerIn: parent
                running: true

            }

        }
    }
}
