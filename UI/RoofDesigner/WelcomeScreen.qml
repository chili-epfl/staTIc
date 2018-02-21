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
                folder: "file:"+scenariosPath
                nameFilters: ["*.json"]
            }
            delegate:Rectangle{
                width: height*4/3
                height: welcome_page.availableHeight/3
                border.color: "black"
                border.width: 3
                Item{
                    enabled: false
                    anchors.fill: parent
                    JSONSketch{
                        id:json_sketch
                    }
                    Component.onCompleted: {
                        json_sketch.loadSketch(scenariosPath,fileName,this)
                    }
                }
                MouseArea{                  
                    anchors.fill: parent
                    onDoubleClicked: {
                        loading_indicator.visible=true;
                        stack_view.push("qrc:/ui/UI/RoofDesigner/SketchScreen.qml",{"sketch_to_load":fileBaseName});
                        sketch_list_view.model=0
                    }
                    onPressAndHold: {
                        delete_button.visible=true
                    }
                    Button{
                        visible: false
                        id: delete_button
                        anchors.bottom: parent.bottom
                        anchors.margins: 5
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: json_sketch.deleteSketchFile(scenariosPath,fileName);
                        text:"Delete"
                        Timer{
                            interval: 2000
                            repeat: false
                            running: parent.visible
                            onTriggered: parent.visible=false
                        }

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

    Image {
        visible: settings.show_back_button
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        width: 50
        height: 50
        source: "qrc:/icons/Icons/back.png"
        MouseArea{
            anchors.fill: parent
            onClicked:{
                stack_view.parent.pageExit();
            }
        }
    }
}
