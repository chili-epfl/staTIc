import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    property alias selection:  root.state

    id:root
    width: icon_model.count*(74)
    height: 84
    radius:5
    color: "#606060"
    border.color: "#F0F0F0"
    state:"DESIGNER"

    states:[
        State {
            name: "BEAM"
            PropertyChanges {
                target: infobox
                current_item: 0
            }
            PropertyChanges {
                target: infobox
                lateral_visibility: "Visible"
            }
        },
        State{
            name:"JOINT"
            PropertyChanges {
                target: infobox
                current_item: 0
            }
        },
        State{
            name:"DESIGNER"
            PropertyChanges {
                target: infobox
                current_item: 0
            }
            PropertyChanges {
                target: infobox
                lateral_visibility: "Visible"
            }
        }
    ]

    ListModel {
        id:icon_model
        ListElement {
            name: "DESIGNER"
            imageSource: "qrc:/images/Images/designer.png"
        }
        ListElement {
            name: "BEAM"
            imageSource: "qrc:/images/Images/analysis_mode.png"
        }

//        ListElement {
//            name: "JOINT"
//            imageSource: "qrc:/images/Images/wood_joint.png"
//        }

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
                    onClicked: {layoutView.currentIndex = index;root.state=name;infobox.current_item=0}
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 ; border.color: "red"}
        }
}


}
