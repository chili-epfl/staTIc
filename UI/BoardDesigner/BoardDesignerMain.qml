import QtQuick 2.6
import QtQuick.Window 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import Qt.labs.folderlistmodel 2.1
import IOBoardFile 1.0
Rectangle {
    border.color: "#F0F0F0"
    border.width: 10
    color: "#2f3439"
    visible: true
    anchors.fill: parent
    signal pageExit();
    anchors.margins: 10
    ColumnLayout {
        anchors.rightMargin: 0
        anchors.bottomMargin: 10
        anchors.leftMargin: 0
        anchors.topMargin: 100
        visible: loader.status!=Loader.Ready
        anchors.fill: parent
        spacing: 20
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: create_button.height
            Button{
                id:create_button
                anchors.centerIn: parent
                text:"Create a new board"
                onClicked: loader.source="qrc:/ui/UI/BoardDesigner/CreateBoard.qml"
            }
        }
        Item{
            Layout.fillHeight: true
            Layout.fillWidth: true
            Button{
                id:open_button
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.margins: 10
                text:"Open board"
                enabled: file_view.currentIndex>-1
                onClicked: {
                     loader.source="qrc:/ui/UI/BoardDesigner/OpenBoard.qml"
                }
            }
            ListView {
                id:file_view
                anchors.margins: 10
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: open_button.bottom
                anchors.bottom: parent.bottom
                width: 0.8*parent.width;

                clip:true

                ScrollBar.vertical: ScrollBar {
                    parent: file_view.parent
                    anchors.top: file_view.top
                    anchors.left: file_view.right
                    anchors.bottom: file_view.bottom
                }
                FolderListModel {
                    id: folderModel
                    showDirs: false
                    folder: "file:"+boardsPath
                    nameFilters: ["*.data"]
                }

                Component {
                    id: fileDelegate
                    Rectangle{
                        width: 0.8*file_view.parent.width
                        height: file_name_text.font.pixelSize+10
                        color:file_view.currentIndex==index?"lightsteelblue":"#2f3439"
                        Text {
                            color: file_view.currentIndex==index?"black":"#F0F0F0"
                            id:file_name_text
                            text: fileName
                            font.bold: true
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: file_view.currentIndex=index
                        }
                    }
                }

                model: folderModel
                delegate: fileDelegate
            }

        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: delete_button.height+20
            Button{
                id:delete_button
                anchors.centerIn: parent
                text:"Delete board"
                enabled: file_view.currentIndex>-1
                onClicked: {
                    if(text=="Delete board"){
                        text="Sure?"
                        delete_timer.restart();
                    }
                    else{
                        file_deleter.removeFile(folderModel.get(file_view.currentIndex,"fileName"))
                        text="Delete board"
                        delete_timer.stop()
                    }
                }
                Timer{
                    id:delete_timer
                    interval: 3000
                    repeat: false
                    onTriggered: delete_button.text="Delete board"
                }
            }
        }
    }

    Loader{
        id:loader
        clip: true
        anchors.centerIn: parent
        property bool valid: item !== null
        //height: Qt.platform.os=="android" ? parent.height : 480*width/640
        //width: Qt.platform.os=="android" ? parent.width : (480*parent.width/640<=parent.height)? parent.width : parent.height*640/480
        anchors.fill: parent
        anchors.margins: 10
        onLoaded: if(source=="qrc:/ui/UI/BoardDesigner/OpenBoard.qml"){
                    item.file_path=folderModel.get(file_view.currentIndex,"fileName");
                  }

    }

    Connections {
        ignoreUnknownSignals: true
        target: loader.valid ? loader.item : null
        onPageExit: {
            loader.source=""
        }
    }

    IOBoardFile{
        id:file_deleter
    }

    Image{
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.margins: 20
        width: Screen.pixelDensity*10
        fillMode: Image.PreserveAspectFit
        source: "qrc:/icons/Icons/back.png"
        MouseArea{
            anchors.fill: parent
            onClicked: loader.source!="" ? loader.source="" : pageExit()
        }
    }

}
