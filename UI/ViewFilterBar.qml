import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    property alias selection:  root.state

    id:root
    width: icon_model.count*(74)
    height: 84
    radius:5

    state:"BEAM"

    states:[
        State {
            name: "BEAM"
        },
        State{
            name:"JOINT"
        }
    ]

    ListModel {
        id:icon_model
        ListElement {
            name: "BEAM"
            imageSource: "qrc:/images/Images/wood_beam.png"
        }

        ListElement {
            name: "JOINT"
            imageSource: "qrc:/images/Images/wood_joint.png"
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
                    onClicked: {layoutView.currentIndex = index;root.state=name;}
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }
}


}
