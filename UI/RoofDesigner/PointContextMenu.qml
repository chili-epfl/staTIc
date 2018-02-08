import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0

Popup {
    id: pointContextMenu
    width: Screen.pixelDensity*10 + 20
    height: Screen.pixelDensity*10 + 20
    visible: false

    property bool origin: o.checked
    /*property bool cx: cx.checked
    property bool cy: cy.checked
    property bool cz: cz.checked
    property bool mx: mx.checked
    property bool my: my.checked
    property bool mz: mz.checked*/

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ToolButton {
            id: o
            text: "Origin"
            font.family: "FontAwesome"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                if (sketch.origin != null){
                    sketch.origin.color = "blue"
                }
                sketch.origin = _root
                _root.color = "green"
            }
        }

        /*ToolButton {
            id: cx
            text: "\uf023 x"
            font.family: "FontAwesome"
            checkable: true
        }
        ToolButton {
            id: cy
            text: "\uf023 y"
            font.family: "FontAwesome"
            checkable: true
        }
        ToolButton {
            id: cz
            text: "\uf023 z"
            font.family: "FontAwesome"
            checkable: true
        }

        ToolButton {
            id: mx
            text: "\uf01e x"
            font.family: "FontAwesome"
            checkable: true
        }
        ToolButton {
            id: my
            text: "\uf01e y"
            font.family: "FontAwesome"
            checkable: true
        }
        ToolButton {
            id: mz
            text: "\uf01e z"
            font.family: "FontAwesome"
            checkable: true
        }*/
    }
}

