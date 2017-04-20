import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import "qrc:/ui/UI"

Rectangle {
    id:root

    property ListModel question_beams;

    color: "transparent"


    ColumnLayout{
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        Rectangle{
            color:"transparent"
            border.color: "#F0F0F0"
            border.width: 5
            radius:1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.10*root.height
            Rectangle{
                color:"#2f3439"
                radius: 1
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    wrapMode: Text.WordWrap
                    anchors.fill: parent
                    anchors.margins: 10
                    id: title
                    text: qsTr("Identifiez le type de force axiale dans chaque élément")
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10; font.pixelSize: 32
                    color: "white"
                }
            }
        }
        Rectangle{
            color:"transparent"
            border.color: "#F0F0F0"
            border.width: 5
            radius:1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.50*root.height
            ListView{
                id:list_beams
                anchors.fill: parent
                model: question_beams
                spacing:10
                anchors.margins: 10
                clip:true
                delegate: Rectangle{
                    color:"#2f3439"
                    width: root.width-20
                    height: (Screen.pixelDensity*10)
                    anchors.margins: 10
                    RowLayout{
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        Item{
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width/2
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                            Text{
                                anchors.centerIn: parent
                                color:"#F0F0F0"
                                id:beam_label
                                text:beam.objectName
                                fontSizeMode: Text.Fit
                                minimumPixelSize: 10; font.pixelSize: 32
                            }
                        }
                    }
                }
            }
        }

    }
}


