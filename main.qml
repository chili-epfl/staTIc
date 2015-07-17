import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import StructureItemModel 1.0

ApplicationWindow {
    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true
    width: 2560
    height: 1600
    color: "#0033CC"

    /*Two variables hold the related files of the selected structure*/
    property string currentStructure_modelName: "";
    property url currentStructure_modelFile: "";
    property url currentStructure_staticsFile: "";
    property url currentStructure_tagFile: "";

    Loader{
        id:windowloader
        property bool valid: item !== null
        anchors.fill: parent
        onLoaded: {
            intromenu.visible=false;
            intromenu.enabled=false;
            console.log("loaded");
            item.loadStructure(currentStructure_modelName,currentStructure_modelFile,currentStructure_staticsFile,currentStructure_tagFile);
        }
    }

    Connections {
        ignoreUnknownSignals: true
        target: windowloader.valid? windowloader.item : null
        onPageExit: { intromenu.visible=true;intromenu.enabled=true;windowloader.source=""}
    }

    /*Item conteining all the item for the intro menu.*/
    ColumnLayout{
        anchors.fill: parent
        anchors.margins:10
        id:intromenu
        spacing: 5
        Item{
            id:banner
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.2
            Layout.alignment: Qt.AlignCenter

            Text{
                text: "StaTIc"
                font.pixelSize: pt2px(20)
            }
        }

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
                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredHeight:  gridRect.height-2*caption.font.pixelSize;
                        Layout.preferredWidth: gridRect.height;
                    Image {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: decoration;
                    }
                    }
                    Item{
                        Layout.alignment: Qt.AlignCenter
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
                //cellHeight: height/2
                //cellWidth: width/4
                anchors.fill: parent
                model: StructureItemModel{ source:":/models/models" }
                highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
                delegate:modelDelegate

                flickableChildren: MouseArea {
                    anchors.fill: parent
                    onClicked: gridview.currentIndex = -1
                }
                onCurrentIndexChanged: console.log(currentIndex)

            }
        }
        Item{
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height*0.1
            Button {
                text: "Button"
                onClicked: {
                    if(gridview.currentIndex !=-1){
                        currentStructure_modelName= gridview.model.get(gridview.currentIndex,"Name");
                        currentStructure_modelFile= gridview.model.get(gridview.currentIndex,"ModelFile");
                        currentStructure_staticsFile= gridview.model.get(gridview.currentIndex,"StaticsFile");
                        currentStructure_tagFile= gridview.model.get(gridview.currentIndex,"TagFile");
                        windowloader.source="exploreView.qml"
                        }
                    }
                anchors.right: parent.right
                anchors.rightMargin: 10
            }
        }
    }


}
