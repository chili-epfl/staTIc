import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import Chilitags 1.0
import Controller 1.0

Item{
    id: root
    signal exitView()
    signal loadStructure(string modelFile,string staticsFile);

    onLoadStructure: controller.loadStructure(modelFile,staticsFile);

    anchors.fill: parent
    state: "AG"
    states:[State {
            name: "AG"
            StateChangeScript {
                name: "stopcamera"
                script: camDevice.start();
            }
        },
        State {
            name: "V"
            StateChangeScript {
                name: "stopcamera"
                script: camDevice.stop();
            }        }
    ]

    Controller{
        id:controller
        sceneRoot: scene.sceneroot

    }
    /*UI*/
    Item{
        id:topmenu
        anchors.horizontalCenter: parent.horizontalCenter
        y:0 ; z:1
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
        z:1
        anchors.verticalCenter: parent.verticalCenter
        ColumnLayout{
            spacing: 0
            Rectangle{
                color: (root.state=="AG")? "red": "yellow"
                width: Math.max(captionVirtual.font.pixelSize,captionAG.font.pixelSize) +10
                height: width
                radius: 5
                Text{
                    id: captionAG
                    text: "AG"
                    font.pixelSize: pt2px(11);
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.state="AG"
                    }
                }
            }
            Rectangle{
                color: (root.state=="V")? "red" : "yellow"
                width: Math.max(captionVirtual.font.pixelSize,captionAG.font.pixelSize) +10
                height: width
                radius: 5
                Text{
                    id: captionVirtual
                    text: "V"
                    font.pixelSize: pt2px(11);
                    anchors.centerIn: parent
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.state="V"
                    }
                }
            }
        }



    }

    /*3D Rendering*/

    Camera{
        id:camDevice
        imageCapture.resolution: "640x480" //Android sets the viewfinder resolution as the capture one
        //viewfinder.resolution:"640x480"
    }

    VideoOutput{
        z: 0
        anchors.centerIn: parent
        anchors.fill: parent
        source:camDevice
        filters:[chilitags]
        Scene3D {
                anchors.fill: parent
                focus: true
                aspects: "input"
                Scene {
                    id:scene
                }
            }
    }

    Chilitags{
        id:chilitags
        chiliobjects: [tag]

    }

    ChilitagsObject{
        id: tag
        name: "tag_1018"
    }




}
