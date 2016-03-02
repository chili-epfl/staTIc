import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import Chilitags 1.0

import Qt3D.Render 2.0

import BackgroundSubtraction 1.0

import Frame3DDKernel 1.0
import Frame3DDVMManager 1.0

import "qrc:/ui/UI/"
import Warehouse3D 1.0
Item{
    id: applicationRoot
    anchors.fill: parent
    property alias currentViewFilter: viewFilterBar.selection

    property alias maxForce : staticsmodule.maxForce
    property alias minForce : staticsmodule.minForce

    property bool firstInit : true

    state: "LoadingCamera"
    states: [
        State {
            name: "LoadingCamera"
            when: firstInit && camDevice.cameraStatus!=Camera.ActiveStatus
            PropertyChanges {
                target: loadingAnimation_text
                text:"Loading Camera"
            }
            PropertyChanges {
                target: loadingAnimation_progressbar
                value:0.25
            }
        },
        State {
            name: "LoadingStaticsModel"
            when: firstInit && camDevice.cameraStatus==Camera.ActiveStatus &&
                  staticsmodule.status!=Frame3DDKernel.LOADED
            PropertyChanges {
                target: loadingAnimation_text
                text:"Loading 3D structure"
            }
            PropertyChanges {
                target: loadingAnimation_progressbar
                value:0.5
            }
        },
        State {
            name: "Loading3DModel"
            when: firstInit && staticsmodule.status==Frame3DDKernel.LOADED &&
                  scene3D.structureLoader.status!=SceneLoader.Loaded
            PropertyChanges {
                target: loadingAnimation_text
                text:"Loading Scenario"
            }
            PropertyChanges {
                target: loadingAnimation_progressbar
                value:0.75
            }

        },
        State {
            name: "LoadingVMManager"
            when: firstInit && scene3D.structureLoader.status==SceneLoader.Loaded
            PropertyChanges {
                target: loadingAnimation_text
                text:"Enjoy :)"
            }
            PropertyChanges {
                target: vmFrame3DDManager
                staticsModule:staticsmodule
            }
            PropertyChanges {
                target: vmFrame3DDManager
                sceneRoot:scene3D.structureEntity
            }
            PropertyChanges {
                target: loadingAnimation_progressbar
                value:1
            }
            StateChangeScript {
                script: {
                    loadingAnimation.destroy(2000)
                    firstInit=false;
                }
            }
        }
    ]

    Component.onCompleted: camDevice.deviceId=QtMultimedia.availableCameras[1].deviceId

    /*Loading animation*/
    Rectangle{
        id:loadingAnimation
        anchors.fill: parent
        color: "#2f3439"
        z:2
        Image {
            anchors.centerIn: parent
            id: loadingLogo
            width: parent.width/3
            height: width
            source:"qrc:/icons/Icons/LOADING.png"
        }
        Text{
           id:loadingAnimation_text
           anchors.horizontalCenter: loadingAnimation.horizontalCenter
           anchors.top: loadingLogo.bottom
           text: "Loading Camera"
           color: "white"
           font.pixelSize: pt2px(15)
        }
        ProgressBar{
            id:loadingAnimation_progressbar
            anchors.margins: 10
            anchors.horizontalCenter: loadingAnimation.horizontalCenter
            anchors.top: loadingAnimation_text.bottom
        }
    }
    /************/

    Frame3DDKernel{
        id:staticsmodule
        onStatusChanged: {
            if(firstInit && status==Frame3DDKernel.LOADED){
                loadingAnimation_text.text="Loading Scenario";
                scene3D.structureLoader.source="qrc:/scenarios/Scenarios/Scenario_1/Model1.dae"
            }
        }
    }

    property alias materialsManager: staticsmodule.materialsManager

    Frame3DDVMManager{
            id:vmFrame3DDManager
    }
    Warehouse3D{
        id:warehouse3d
    }

     /*3D Rendering*/
     Camera{
         id:camDevice
         onCameraStatusChanged: {
             if(firstInit && camDevice.cameraStatus==Camera.ActiveStatus ){
                 loadingAnimation_text.text="Loading 3D structure";
                 staticsmodule.source=":/scenarios/Scenarios/Scenario_1/Model1.lol"
             }
         }
         imageCapture.resolution: "640x480" //Android sets the viewfinder resolution as the capture one
         viewfinder.resolution:"640x480"
         focus {
                     focusMode: Camera.FocusMacro
                     focusPointMode: Camera.FocusPointAuto
                 }
         imageProcessing {
                 whiteBalanceMode: CameraImageProcessing.WhiteBalanceAuto
         }
         exposure.exposureMode: Camera.ExposureAction
         exposure.manualAperture: -1
         exposure.manualShutterSpeed: -1

         imageCapture {
             onImageCaptured: {
                 stillImage.source = preview
             }
         }
         property bool isRunning: true
         onIsRunningChanged: {
            if(isRunning){
                camDevice.start();
            }
            else{
                camDevice.imageCapture.capture();
                camDevice.stop();
            }
         }
     }

     Image {
         id: stillImage
         visible: camDevice.isRunning && camDevice.cameraStatus==Camera.ActiveStatus ? false:true
         anchors.fill: parent
         fillMode: Image.PreserveAspectFit
     }

     VideoOutput{
         z: 0
         id:viewFinder
         anchors.centerIn: parent
         anchors.fill: parent
         source: camDevice
         filters:[chilitags]

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
                 aspects: ["input","physics","logic"]
                 //aspects:["input"]
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
                    id:infobox
                    width:Math.max(parent.width/3,pt2px(142))//5cm
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: pt2px(20)
                }

                Image {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: camDevice.isRunning ?
                            "qrc:/icons/Icons/AR_ON.png" :
                            "qrc:/icons/Icons/AR_OFF.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: camDevice.isRunning = !camDevice.isRunning
                    }
                }
             }
         }
     }

     Chilitags{
         id:chilitags
         tagConfigurationFile:":/scenarios/Scenarios/Scenario_1/Model1.yml"
         chiliobjects: [structure_tag]
         //pause: backgroundsubtraction.entropy > .01 ? true : false
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
