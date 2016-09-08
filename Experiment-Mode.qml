import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import ScenarioListModel 1.0
import ResourcesFetcher 1.0
import QtQuick.Dialogs 1.2
import QtSensors 5.3
import "qrc:/ui"
import "qrc:/scripts/Scripts"
import "qrc:/ui/UI/Help"
Item {

    property bool enable_eye_tracking_tags: false

    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true

    RotationSensor{
        id: rotationSensor
        active: true

    }
//    Accelerometer{
//        id:accelormeter
//        active: true
//    }

    property bool use_custom_board:false

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
            if(item.hasOwnProperty('problem_description_text'))
                item.problem_description_text=stories.list[gridview.currentIndex].story_description;
            if(item.hasOwnProperty('problem_image_url'))
                item.problem_image_url=stories.list[gridview.currentIndex].story_picture;
            if(stories.list[gridview.currentIndex].story_type=="SupportEx"){
                item.loadsOnBeams=stories.list[gridview.currentIndex].loadsOnBeams;
                item.proposed_solution_url=stories.list[gridview.currentIndex].story_suggestion_picture
            }
            if(stories.list[gridview.currentIndex].story_type=="FindAxialEx"){
                item.loadsOnBeams=stories.list[gridview.currentIndex].loadsOnBeams;
                item.hidden_beams=stories.list[gridview.currentIndex].hidden_beams;
                item.question_beams=stories.list[gridview.currentIndex].question_beams
            }
            if(stories.list[gridview.currentIndex].story_type=="Default" && stories.list[gridview.currentIndex].show_demo){
                item.tutorial.interactive=true;
                item.tutorial.visible=true;

            }
        }
    }

    Connections {
        ignoreUnknownSignals: true
        target: scriptLoader.valid ? scriptLoader.item : null
        onPageExit: {
            logger.close_logger();
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
                source: start_button.pressed? "qrc:/icons/Icons/next_pressed.png" :"qrc:/icons/Icons/next.png"
                anchors.right: parent.right
                anchors.bottom:  parent.bottom
                anchors.rightMargin: 10
                MouseArea{
                    id:start_button
                    anchors.fill: parent
                    onClicked: if(gridview.currentIndex !=-1){
                                   logger.restart_logger();
                                    if(stories.list[gridview.currentIndex].story_type=="SupportEx"){
                                        scriptLoader.source="qrc:/scripts/Scripts/SupportExercise.qml";
                                        logger.log("Exercise_SupportEx_Start",{"Story":gridview.currentIndex})
                                    }
                                    else if(stories.list[gridview.currentIndex].story_type=="CableEx"){
                                        scriptLoader.source="qrc:/scripts/Scripts/CableExercise.qml";
                                        logger.log("Exercise_CableEx_Start",{"Story":gridview.currentIndex})
                                    }
                                    else if(stories.list[gridview.currentIndex].story_type=="FindAxialEx"){
                                        scriptLoader.source="qrc:/scripts/Scripts/FindAxialForceExercise.qml";
                                        logger.log("Exercise_FindAxialForce_Start",{"Story":gridview.currentIndex})
                                    }
                                    else if(stories.list[gridview.currentIndex].story_type=="Default"){
                                        scriptLoader.source="qrc:/scripts/Scripts/DefaultScript.qml";
                                        logger.log("Exercise_Default_Start",{"Story":gridview.currentIndex})
                                    }
                               }
                }
            }
            Rectangle{
                visible: enable_eye_tracking_tags
                anchors.horizontalCenter: parent.horizontalCenter
                width: 100
                height: 100
                anchors.bottom: parent.bottom
                Image {
                    id: stop_marker
                    source: "qrc:/ui/UI/Eye_tracking_tags/010.png"
                    anchors.centerIn: parent
                    width: parent.width*0.75
                    height: parent.height*0.75
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

    Rectangle{
        visible: enable_eye_tracking_tags
        width: 100
        height: 100
        Image {
            source: "qrc:/ui/UI/Eye_tracking_tags/012.png"
            anchors.centerIn: parent
            width: parent.width*0.75
            height: parent.height*0.75
        }
    }

    Rectangle{
        visible: enable_eye_tracking_tags
        width: 100
        height: 100
        anchors.top: parent.top
        anchors.right: parent.right
        Image {
            source: "qrc:/ui/UI/Eye_tracking_tags/013.png"
            anchors.centerIn: parent
            width: parent.width*0.75
            height: parent.height*0.75
        }
    }


}
