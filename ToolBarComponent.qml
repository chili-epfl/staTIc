import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    width: icon_model.count*(74)
    height: 84
    color:"grey"
    radius:5

    ListModel {
        id:icon_model
        ListElement {
            name: "Selector"
            imageSource: "qrc:/icons/icons/Force_Icon.svg"
        }
        ListElement {
            name: "Force"
            imageSource: "qrc:/icons/icons/Force_Icon.svg"
        }
        ListElement {
            name: "Delete"
            imageSource: "qrc:/icons/icons/Force_Icon.svg"
        }
    }

    Item{
        anchors.fill: parent
        anchors.margins: 10
    ListView {
        id:toolView
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
                RowLayout{
                Image{
                    id:image
                    Layout.alignment: Qt.AlignCenter
                    anchors.fill: parent
                    anchors.margins: 10
                    source: imageSource

                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: toolView.currentIndex = index
                }
            }
        }
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
    }
}


}
