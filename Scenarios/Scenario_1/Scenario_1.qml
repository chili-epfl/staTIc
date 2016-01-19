import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import Chilitags 1.0

import BackgroundSubtraction 1.0

import Qt3D.Render 2.0

import Frame3DDKernel 1.0
import Frame3DDVMManager 1.0

import "qrc:/ui/UI/"

Item{
    id: applicationRoot
    anchors.fill: parent
    property alias currentViewFilter: viewFilterBar.selection

    property alias maxForce : staticsmodule.maxForce
    property alias minForce : staticsmodule.minForce

    state: "LoadingCamera"
    property  int initState : 0

    states: [
        State {
            name: "LoadingCamera"
            PropertyChanges {
                target: loadingLogo
                source:"qrc:/images/Images/camera_icon.png"
            }
        },
        State {
            name: "LoadingStaticsModel"
            when: camDevice.cameraStatus==Camera.ActiveState && initState<1
            PropertyChanges {
                target: loadingLogo
                source:"qrc:/images/Images/camera_icon.png"
            }
            StateChangeScript{
              script: {
                  initState++;
                  staticsmodule.source=":/scenarios/Scenarios/Scenario_1/Model1.lol"
              }
            }
        },
        State {
            name: "Loading3DModel"
            when: staticsmodule.status==Frame3DDKernel.LOADED && initState<2
            PropertyChanges {
                target: loadingLogo
                source:"qrc:/images/Images/camera_icon.png"
            }
            StateChangeScript{
              script: {
                  initState++;
                  scene3D.structureLoader.source="qrc:/scenarios/Scenarios/Scenario_1/Model1.dae"
              }
            }
        },
        State {
            name: "LoadingVMManager"
            when: scene3D.structureLoader.status==SceneLoader.Loaded && initState<3
            PropertyChanges {
                target: loadingLogo
                source:"qrc:/images/Images/camera_icon.png"
            }
            StateChangeScript {
                script: {
                    initState++
                    vmFrame3DDManager.staticsModule=staticsmodule
                    vmFrame3DDManager.sceneRoot=scene3D.structureEntity
                    loadingAnimation.destroy()
                }
            }
        }
    ]

    /*Loading animation*/
    Rectangle{
        id:loadingAnimation
        anchors.fill: parent
        color: "grey"
        z:2
        Image {
            anchors.centerIn: parent
            id: loadingLogo
            source:"qrc:/images/Images/camera_icon.png"
            Rectangle{
                property real contentSize: Math.max(loadingLogo.paintedHeight,loadingLogo.paintedWidth)
                anchors.centerIn: parent
                width: Math.sqrt(2*contentSize*contentSize)
                height: width
                radius: width*0.5
                color: "transparent"
                border.color: "red"
                border.width: pt2px(10)
            }
        }
        Text{
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: loadingLogo.bottom
           text: "Loading"
           font.pixelSize: pt2px(15)
        }
    }
    /************/

    Frame3DDKernel{
        id:staticsmodule
    }

    Frame3DDVMManager{
            id:vmFrame3DDManager
    }


     /*UI*/
//     Item{
//         id:topmenu
//         anchors.horizontalCenter: parent.horizontalCenter
//         y:0 ; z:1
//         state: "DISPLAYED"
//         ColumnLayout{
//             Row{
//                 ViewFilterBar{
//                     id:viewFilterBar
//                 }
//             }
//             Row{
//                 Layout.alignment: Qt.AlignCenter
//                 Image {
//                     id: ic_topmenu
//                     source: "qrc:/icons/Icons/ic_keyboard_arrow_up_black_36px.svg"
//                     MouseArea{
//                         anchors.fill: parent
//                         onClicked: {
//                             if(topmenu.state=="DISPLAYED")
//                                 topmenu.state="HIDDEN"
//                             else
//                                 topmenu.state="DISPLAYED"
//                         }
//                     }
//                 }

//             }

//         }
//         states: [
//             State {
//                 name: "DISPLAYED"
//                 PropertyChanges { target: topmenu; y: 5}
//                 PropertyChanges { target: ic_topmenu; rotation:0}

//             },
//             State {
//                 name: "HIDDEN"
//                 PropertyChanges { target: topmenu; y: -viewFilterBar.height+10}
//                 PropertyChanges { target: ic_topmenu; rotation:180}

//             }
//         ]
//         Behavior on y {animation: NumberAnimation{duration: 250}}
//     }


     /*3D Rendering*/
     Camera{
         id:camDevice
         deviceId:QtMultimedia.availableCameras[1].deviceId
         imageCapture.resolution: "640x480" //Android sets the viewfinder resolution as the capture one
         viewfinder.resolution:"640x480"

         focus {
                     focusMode: Camera.FocusMacro
                     focusPointMode: Camera.FocusPointAuto
                 }
         imageProcessing {
                 whiteBalanceMode: Camera.WhiteBalanceAuto
         }
         exposure.exposureMode: Camera.ExposureAction
         exposure.manualAperture: -1
         exposure.manualShutterSpeed: -1
     }


     VideoOutput{
         z: 0
         anchors.centerIn: parent
         anchors.fill: parent

         source:camDevice
         filters:[backgroundsubtraction,chilitags]


         Rectangle{
             anchors.fill: parent
             color:"#00000000"
             border.width: 0.125*(2*(parent.width+parent.height)-
                           Math.sqrt(4*(parent.width+parent.height)*(parent.width+parent.height)
                                     -16*(1-0.8)*(parent.width*parent.height))) //80% of screen
             border.color: "#80000000"
             Scene3D {
                 anchors.fill: parent
                 focus: true
                 //aspects: ["input","physics"]
                 aspects:["input"]
                 multisample:true
                 Scenario1Scene3D {
                     id:scene3D
                 }
             }
             Rectangle{
                anchors.centerIn: parent
                width: parent.width -2*parent.border.width
                height:parent.height -2*parent.border.width
                color: "transparent"
                border.color: "#F0F0F0"
                radius:5

                ViewFilterBar{
                    id:viewFilterBar
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                InfoBox{
                    width:Math.max(parent.width/3,pt2px(142))//5cm
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: pt2px(20)
                }

             }
         }

     }

     Chilitags{
         id:chilitags
         tagConfigurationFile:":/scenarios/Scenarios/Scenario_1/Model1.yml"
         chiliobjects: [structure_tag]
         pause: backgroundsubtraction.entropy > .01 ? false : true
     }

     ChilitagsObject{
         id: structure_tag
         name:"Model1"
     }

     BackgroundSubtraction{
        id:backgroundsubtraction
        //onEntropyChanged: console.log(entropy)
     }

}
