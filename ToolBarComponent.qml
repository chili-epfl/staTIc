import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    id:root
    width: icon_model.count*(74)
    height: 84
    radius:5
    state:"SELECT"

    states:[
        State {
            name: "SELECT"
            PropertyChanges { target: toolView; currentIndex: 0 }
        },
        State{
            name:"FORCE"
            PropertyChanges { target: toolView; currentIndex: 1 }

        },
        State{
            name:"DELETE"
            PropertyChanges { target: toolView; currentIndex: 2 }
        }

    ]

    ListModel {
        id:icon_model
        ListElement {
            name: "SELECT"
            imageSource: "qrc:/icons/icons/Selector_Icon.svg"
        }
        ListElement {
            name: "FORCE"
            imageSource: "qrc:/icons/icons/Force_Icon.svg"
        }
        ListElement {
            name: "DELETE"
            imageSource: "qrc:/icons/icons/Delete_Icon.svg"
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
                    onClicked: {toolView.currentIndex = index;root.state=name;}
                }

        }
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
    }
}


}
