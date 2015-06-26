import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.0
import QtQuick.Layouts 1.1

Item{
    id: item1
    signal exitView()
    anchors.fill: parent

    Item{
        id:topmenu
        anchors.horizontalCenter: parent.horizontalCenter
        y:0
        state: "HIDDEN"
        ColumnLayout{
            Row{
                id:toolspace
                Rectangle{width: 120; height: 120}
            }
            Row{
                Layout.alignment: Qt.AlignCenter
                Image {
                    id: ic_topmenu
                    source: "qrc:/icons/icons/ic_keyboard_arrow_up_black_36px.svg"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(topmenu.state=="DISPLAYED")
                                topmenu.state="HIDDEN"
                            else
                                topmenu.state="DISPLAYED"
                        }
                    }
                }


            }

        }

        states: [
            State {
                name: "DISPLAYED"
                PropertyChanges { target: topmenu; y: 5}
                PropertyChanges { target: ic_topmenu; rotation:0}

            },
            State {
                name: "HIDDEN"
                PropertyChanges { target: topmenu; y: -toolspace.height+10}
                PropertyChanges { target: ic_topmenu; rotation:180}

            }
        ]
        Behavior on y {animation: NumberAnimation{duration: 250}}
    }

    Item{
        id:leftmenu
        anchors.verticalCenter: parent.verticalCenter
        ColumnLayout{
            spacing: 0
            Button{
             text: "AG"
             MouseArea{
                 anchors.fill: parent
                 onClicked: {

                 }
             }
            }
            Button{
             text: "V"
             MouseArea{
                 anchors.fill: parent
                 onClicked: {

                 }
             }
            }
        }



    }



}
