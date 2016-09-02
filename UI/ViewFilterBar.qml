import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    property string selection:"DESIGNER"
    id:root
    width: icon_model.count*(76*1.30)
    height: 100*1.30
    radius:5
    color: "#606060"
    border.color: "#F0F0F0"
    visible: settings.show_filter_bar

    ListModel {
        id:icon_model
        ListElement {
            name: "DESIGNER"
            imageSource: "qrc:/images/Images/designer.png"
            label:"Design"
        }
        ListElement {
            name: "ANALYSIS"
            imageSource: "qrc:/images/Images/analysis_mode.png"
            label:"Analyse"
        }
    }

    Item{
        anchors.fill: parent
        anchors.margins: 10
        ListView {
            id:layoutView
            orientation: Qt.Horizontal
            model: icon_model
            spacing:10
            anchors.fill: parent
            delegate:
                Rectangle{
                color: "transparent"
                implicitHeight: 80*1.30
                implicitWidth: 64*1.30
                opacity: 0.7
                Image{
                    id:image
                    Layout.alignment: Qt.AlignCenter
                    fillMode: Image.PreserveAspectFit
                    anchors.topMargin: 10
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.bottomMargin: 5
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    source: imageSource
                    Text {
                        anchors.margins: 5
                        width: parent.width-10
                        text: label
                        font.bold: true
                        font.family: "Arial"
                        color:layoutView.currentIndex == index ? "black":"white"
                        anchors.top: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        fontSizeMode: Text.Fit;
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {layoutView.currentIndex = index;root.selection=name}
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 ; border.color: "red"}
        }
}


}
