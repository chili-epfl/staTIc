import QtQuick 2.3
import QtQuick.Controls 1.3

import QtQuick.Controls 2.3 as QQ2

import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Qt.labs.folderlistmodel 2.1
import JSONSketch 1.0
import "qrc:/ui"
import "qrc:/scripts/Scripts"
import "qrc:/ui/UI/"
import "qrc:/ui/UI/MaterialDesigner"

Item {

    property string board_path: boardSelection_box.boardPath
    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    function mm2px(mm){return mm*Screen.pixelDensity}

    id: applicationWindow
    visible: true

    focus:true
    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
            event.accepted = true
        }
    }

    Image{
        anchors.fill: parent
        fillMode: Image.Tile
        id:background
        source: "qrc:/images/Images/woodbackground.png"
    }

    Loader{
        id:scriptLoader
        property bool valid: item !== null
        asynchronous: false
        anchors.fill: parent
        onLoaded: {
            intromenu.visible=false;
            intromenu.enabled=false;
            item.structureUrl=folder_list_model.get(gridview.currentIndex, "fileURL")
        }
    }

    Connections {
        ignoreUnknownSignals: true
        target: scriptLoader.valid ? scriptLoader.item : null
        onPageExit: {
            intromenu.visible=true;intromenu.enabled=true;scriptLoader.source=""
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

            GridView{
                highlightMoveDuration:400
                id: gridview
                clip:true
                cellWidth: gridRect.width/5; cellHeight: gridRect.height/2
                anchors.fill: parent
                model: folder_list_model
                currentIndex: -1
                FolderListModel {
                    id:folder_list_model
                    showDirs: false
                    folder: "file:"+scenariosPath
                    nameFilters: ["*.json"]
                }
                delegate:
                    Item{
                    width: Math.min(gridview.cellHeight,gridview.cellWidth);
                    height:width
                    Rectangle{
                        id:structure_preview
                        color: "white"
                        border.color: gridview.currentIndex == index ? "red": "transparent"
                        border.width: 10
                        anchors.fill: parent
                        anchors.margins: 10
                        property bool valid: true
                        JSONSketch{
                            id:json_sketch
                        }
                        Component.onCompleted: {
                            var res=json_sketch.loadSketch(scenariosPath,fileName,this)
                            if(res=="Sketch read" || res=="3D Sketch")
                                valid=true
                            else
                                valid=false
                        }
                        property alias background_picture_url: backgroundImage.source
                        Image {
                            z:2
                            id: backgroundImage
                            mipmap: true
                            anchors.fill: parent
                            anchors.margins: 10
                            fillMode: Image.PreserveAspectCrop
                        }
                        Text {
                            z:3
                            anchors.fill: parent
                            text: structure_preview.valid ? index:"Invalid Format"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment:  Text.AlignVCenter
                            fontSizeMode: Text.Fit
                            minimumPixelSize: 10; font.pixelSize: 50
                            color: "black"
                            wrapMode: Text.WordWrap
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: if(structure_preview.valid) gridview.currentIndex = index
                        }
                    }
                }
            }
        }

        /*Button*/
        Item{
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.1
            Flickable{
                anchors.centerIn: parent
                width: Math.min(parent.width-2*start_button_img.width,row_of_buttons.implicitWidth)
                height: parent.height
                clip: true
                contentWidth: row_of_buttons.implicitWidth
                contentHeight: parent.height
                flickableDirection: Flickable.HorizontalFlick
                Row{
                    id: row_of_buttons
                    height: parent.height
                    spacing: mm2px(2.5)
                    QQ2.RoundButton{
                        text:"Synch on Realto"
                    }
                    QQ2.RoundButton{
                        text:"Select Board"
                        onClicked: boardSelection_box.visible=true
                    }
                    QQ2.RoundButton{
                        text:"Manage Materials"
                        onClicked: aux_loader.source="qrc:/ui/UI/MaterialDesigner/MaterialDesigner.qml"
                    }
                    QQ2.RoundButton{
                        text:"Manage Structures"
                        onClicked: aux_loader.source="qrc:/ui/UI/RoofDesigner/RoofDesignerMain.qml"
                    }
                    QQ2.RoundButton{
                        text:"Manage Boards"
                        onClicked: aux_loader.source="qrc:/ui/UI/BoardDesigner/BoardDesignerMain.qml"
                    }

                }
            }
            Image{
                id:start_button_img
                width: 100
                height: 100
                source: start_button.pressed? "qrc:/icons/Icons/next_pressed.png" :"qrc:/icons/Icons/next.png"
                anchors.right: parent.right
                anchors.bottom:  parent.bottom
                anchors.rightMargin: 10
                MouseArea{
                    id:start_button
                    anchors.fill: parent
                    onClicked:{ if(gridview.currentIndex !=-1 && board_path!=""){
                            scriptLoader.source="qrc:/scripts/Scripts/DefaultScript.qml";
                        }
                        else if(board_path==""){
                            main_suggestion_box.show_alert_message("Select a valid board!")
                        }
                    }
                }
            }
        }
    }

    Loader{
        property bool valid: item !== null
        id:aux_loader
        anchors.fill: parent

    }
    Connections {
        ignoreUnknownSignals: true
        target: aux_loader.valid ? aux_loader.item : null
        onPageExit: {
            aux_loader.source=""
        }
    }


    BoardSelection{
        id:boardSelection_box
        visible:false
    }

    SuggestionBox{
        id:main_suggestion_box
    }




}
