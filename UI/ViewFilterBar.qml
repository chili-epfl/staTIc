import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    property string selection:"DESIGNER"
    id:root
    width: icon_model.count*(74)
    height: 84
    radius:5
    color: "#606060"
    border.color: "#F0F0F0"
    visible: settings.show_filter_bar

    ListModel {
        id:icon_model
        ListElement {
            name: "DESIGNER"
            imageSource: "qrc:/images/Images/designer.png"
        }
        ListElement {
            name: "ANALYSIS"
            imageSource: "qrc:/images/Images/analysis_mode.png"
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
                implicitHeight: 64
                implicitWidth: 64
                opacity: 0.7
                Image{
                    id:image
                    Layout.alignment: Qt.AlignCenter
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    anchors.margins: 10
                    source: imageSource

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
