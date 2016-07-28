import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import ScenarioListModel 1.0
import ResourcesFetcher 1.0
import QtQuick.Dialogs 1.2
import "qrc:/ui"
import "qrc:/scripts/Scripts"
Item {

    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true

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

    Stories{
        id:stories
    }

    Loader{
        id:scriptLoader
        property bool valid: item !== null
        asynchronous: false
        anchors.fill: parent
        onLoaded: {
            intromenu.visible=false;
            intromenu.enabled=false;
            item.structureUrl=stories.list[gridview.currentIndex].structure_url;
            item.problem_description_text=stories.list[gridview.currentIndex].story_description;
            item.problem_image_url=stories.list[gridview.currentIndex].story_picture;
            if(stories.list[gridview.currentIndex].story_type=="SupportEx"){
                item.loadsOnBeams=stories.list[gridview.currentIndex].loadsOnBeams;
                item.proposed_solution_url=stories.list[gridview.currentIndex].story_suggestion_picture
            }
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
            GridView{
                highlightMoveDuration:400
                id: gridview
                clip:true
                cellWidth: gridRect.width/5; cellHeight: gridRect.height/2
                anchors.fill: parent
                model: stories.list.length
                delegate: Item{
                    width: Math.min(gridview.cellHeight,gridview.cellWidth); height:width
                    Image {
                        source: stories.list[index].story_picture
                        fillMode: Image.PreserveAspectCrop
                        anchors.fill: parent
                        anchors.margins: 10
                    }
                    Rectangle{
                    color: "#AA2f3439"
                    border.color: gridview.currentIndex == index ? "red": "transparent"
                    border.width: 10
                    anchors.fill: parent
                    anchors.margins: 10
                    Text {
                        anchors.fill: parent
                        text: index
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:  Text.AlignVCenter
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        color: "#F0F0F0"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: gridview.currentIndex = index
                    }
                }}
            }
        }

        /*Button*/
        Item{
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.1

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
                anchors.right: parent.right
                anchors.bottom:  parent.bottom
                anchors.rightMargin: 10
                MouseArea{
                    anchors.fill: parent
                    onClicked: if(gridview.currentIndex !=-1){
                                    if(stories.list[gridview.currentIndex].story_type=="SupportEx"){
                                        scriptLoader.source="qrc:/scripts/Scripts/SupportExercise.qml";
                                    }
                                    else if(stories.list[gridview.currentIndex].story_type=="CableEx"){
                                        scriptLoader.source="qrc:/scripts/Scripts/CableExercise.qml";
                                    }
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
