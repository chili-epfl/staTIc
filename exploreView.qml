import QtQuick 2.0
import QtQuick.Controls 1.1
import Qt3D 1.0

Item{
    id: item1
    signal exitView()

    Rectangle{
        id:backgroud
        color: "red"
        anchors.fill: parent
    }

    Column {
        id: column1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0

        TextField {
            id: textField1
            placeholderText: qsTr("Text Field")
        }
    }


}
