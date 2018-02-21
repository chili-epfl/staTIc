import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import Qt.labs.folderlistmodel 2.1

Rectangle{
    id: root
    visible: false;
    radius: 5
    border.color: "#F0F0F0"
    border.width: 10
    color: "transparent"
    width: parent.width/3
    height: parent.height/3
    anchors.centerIn: parent
    anchors.margins: 10

    property string boardPath: file_view.currentIndex>-1 ?
                                  folderModel.get(file_view.currentIndex,"filePath"):
                                  ""
    Rectangle{
        anchors.fill: parent
        anchors.margins: 20
        color: "#2f3439"
        ColumnLayout{
            anchors.fill:parent
            anchors.margins: 10
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                ListView {
                    id:file_view
                    anchors.margins: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
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
                        onCountChanged: folderModel.count>0 ? file_view.currentIndex=0 : file_view.currentIndex=-1
                    }

                    Component {
                        id: fileDelegate
                        Rectangle{
                            width: 0.8*file_view.parent.width
                            height: file_name_text.font.pixelSize+10
                            color:file_view.currentIndex==index?"lightsteelblue":"white"
                            Text {
                                id:file_name_text
                                text: fileName
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: file_view.currentIndex==index?
                                             file_view.currentIndex=-1
                                             :file_view.currentIndex=index
                            }
                        }
                    }

                    model: folderModel
                    delegate: fileDelegate
                }
            }
            Item{
                Layout.preferredHeight: close_button.implicitHeight
                Layout.fillWidth: true
                Button{
                    anchors.centerIn: parent
                    id:close_button
                    text:"Close"
                    onClicked: root.visible=false
                }
            }
        }
    }
}
