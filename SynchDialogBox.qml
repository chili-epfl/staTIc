import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

Rectangle{
    property var resourcesFetcher;

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

    Rectangle{
        anchors.fill: parent
        anchors.margins: 20
        color: "#2f3439"
        Image{
            id:logo_image
            source: "qrc:/icons/Icons/realto_logo.png"
            height: parent.height/3 - 20
            width: height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top:parent.top
            anchors.margins: 20
        }
        TextField {
            id:email_field
            placeholderText: "Enter Email"
            text:"lorenzo.lucignano@epfl.ch"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: logo_image.bottom
            anchors.margins: 20
            font.pointSize:15
            width: parent.width-40

        }
        TextField {
            id:host_field
            placeholderText: "Enter Realto Host"
            text: "http://localhost:3003/api/carpenterData"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: email_field.bottom
            font.pointSize:15
            anchors.margins: 20
            width: parent.width-40

        }

        Button{
            text: "Synchronise"
            onClicked: {
                resourcesFetcher.username=email_field.text
                resourcesFetcher.hostUrl=Qt.resolvedUrl(host_field.text)
                resourcesFetcher.synchronize();
            }
            anchors.top: host_field.bottom
            anchors.left: cancel.right
            anchors.margins: 20
        }
        Button{
            id:cancel
            text:"Close"
            anchors.top: host_field.bottom
            anchors.right:parent.horizontalCenter
            onClicked: root.visible=false
            anchors.margins: 20
        }
        ProgressBar {
            id:progress_bar
            visible: resourcesFetcher.loading
            width: parent.width-40
            anchors.top: cancel.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 20
            indeterminate: true
        }
    }
}
