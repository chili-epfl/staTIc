import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle{
    id: root
    visible: false;
    radius: 5
    border.color: "#F0F0F0"
    border.width: 10
    color: "transparent"
    width: parent.width/3
    height:parent.height/3
    anchors.centerIn: parent
    anchors.margins: 10

    Rectangle{
        anchors.fill: parent
        anchors.margins: 20
        anchors.centerIn: parent
        color: "#2f3439"
        Image{
            id:texture_image
            height: width
            width: parent.width/2 - 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.horizontalCenter
            anchors.margins: 20
        }
        Item{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.margins: 20
            width: parent.width/2 - 20
            height: parent.height
            Column{
                anchors.centerIn: parent
                anchors.margins: 20
                spacing: 10
                TextField {
                    id:name_field
                    placeholderText: "Enter Name"
                    font.pointSize:15
                    width: parent.width-10

                }
                TextField {
                    id:density_field
                    placeholderText: "Enter Density"
                    font.pointSize:15
                    width: parent.width-10
                }
                TextField {
                    id:price_field
                    placeholderText: "Enter Price"
                    font.pointSize:15
                    width: parent.width-10
                }
                TextField {
                    id:young_field
                    placeholderText: "Enter Elasticity Module"
                    font.pointSize:15
                    width: parent.width-10
                }
                TextField {
                    id:g_field
                    placeholderText: "Enter Shear Module"
                    font.pointSize:15
                    width: parent.width-10
                }
                Row{
                    spacing: 20
                    Button{
                        text: "Create"
                        onClicked: {

                        }
                    }
                    Button{
                        text: "Create"
                        onClicked: {

                        }

                    }


                }
            }


        }

    }

}

