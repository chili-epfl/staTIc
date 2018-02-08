import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import Constraints 1.0

ToolBar {
    id: topBar
    property alias sketch_name: title_field.text
    TextField{
        id:title_field
        background: Rectangle{color: "transparent"}
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 15
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        placeholderText: "Title: "+ default_title
        property string default_title
        Component.onCompleted: {
            var date=new Date()
            default_title= date.toLocaleString(Qt.locale("en_GB"),"dd-MMM-yy_hh-mm")
        }
    }
    Row{
        anchors.centerIn: parent
        spacing: 10
        Image{
            source: "qrc:/ui/UI/RoofDesigner/icons/simple-orange-house-md.png"
            fillMode: Image.PreserveAspectFit
            height: topBar.height-10
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //Add code to save
                    title_field.text.trim().length>0 ?
                                json_sketch.exportJSONSketch(scenariosPath+title_field.text.trim()+".json", sketch, 0):
                                json_sketch.exportJSONSketch(scenariosPath+title_field.default_title+".json", sketch, 0)
                    stack_view.pop()
                }
            }
        }
        ToolSeparator{height: topBar.height}
        Image{
            source: "qrc:/ui/UI/RoofDesigner/icons/undo-4-xxl.png"
            fillMode: Image.PreserveAspectFit
            height: topBar.height-10
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: sketch.undo();
            }
        }
        Image{
            source: "qrc:/ui/UI/RoofDesigner/icons/redo-4-xxl.png"
            fillMode: Image.PreserveAspectFit
            height: topBar.height-10
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: sketch.redo();
            }
        }
        ToolSeparator{height: topBar.height}
        Image{
            source: "qrc:/ui/UI/RoofDesigner/icons/camera.png"
            fillMode: Image.PreserveAspectFit
            height: topBar.height-10
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: sketchScreen.aux_loader.source="qrc:/ui/UI/RoofDesigner/CaptureImage.qml"

            }
        }
        ToolSeparator{height: topBar.height}
        Image{
            source: "qrc:/ui/UI/RoofDesigner/icons/export3d.png"
            fillMode: Image.PreserveAspectFit
            height: topBar.height-10
            anchors.verticalCenter: parent.verticalCenter
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    title_field.text.trim().length>0 ?
                                json_sketch.exportJSONSketch(scenariosPath+title_field.text.trim()+".json", sketch, 0):
                                json_sketch.exportJSONSketch(scenariosPath+title_field.default_title+".json", sketch, 0)

                }
            }
        }
        ToolSeparator{height: topBar.height}
        ToolButton{
            text:"⋮"
            height: topBar.height
            onClicked: textMenu.open()
            Menu {
                id:textMenu
                title: "View"
                y:parent.height
                width: display_background_menu_item.implicitWidth
                MenuItem {
                    id:display_background_menu_item
                    text: "Display background"
                    enabled: sketchScreen.isBackgroundSet
                    checkable: true
                    checked: sketchScreen.isBackgroundSet && sketchScreen.visibleBackground
                    onTriggered: {
                        sketchScreen.visibleBackground=this.checked
                    }
                }
                MenuItem {
                    text: "Display grid"
                    checkable: true
                    checked: sketchScreen.visibleGrid
                    onTriggered: {
                        sketchScreen.visibleGrid=this.checked
                    }
                }
                MenuItem {
                    text: "Export sketch in mm"
                    onClicked: {
                        title_field.text.trim().length>0 ?
                                    json_sketch.exportJSONSketch(scenariosPath+title_field.text.trim()+".json", sketch, 1):
                                    json_sketch.exportJSONSketch(scenariosPath+title_field.default_title+".json", sketch, 1)

                    }
                }
                /*MenuItem {
                    text: "Login"
                    onTriggered: {
                        loginFormLoader.source = "qrc:/LoginForm.qml"
                        sketchScreenLoader.source = ""
                    }
                }*/
            }
        }
    }
}




