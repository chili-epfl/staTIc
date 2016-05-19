import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import ArucoDetector 1.0
import ArucoObject 1.0
import QuaternionHelper 1.0
import Qt3D.Render 2.0

import BackgroundSubtraction 1.0

import Frame3DDKernel 1.0
import Frame3DDVMManager 1.0

import "qrc:/ui/UI/"
import Warehouse3D 1.0
Item{
    id: applicationRoot
    anchors.fill: parent
    signal pageExit();
    property alias currentViewFilter: viewFilterBar.selection

    property alias maxForce : staticsmodule.maxForce
    property alias minForce : staticsmodule.minForce

    property bool firstInit : true

    property url structureUrl;
    property url structure3DAsset;
    property url structureTagConfig;
    QuaternionHelper{
        id:quaternion_helper
    }

    property real text_color_alpha:0.0
    SequentialAnimation{
        id:text_animation
        NumberAnimation{
            duration:200
            easing: Easing.OutExpo
            target: applicationRoot
            property:"text_color_alpha"
            from:0.0
            to:1
        }
        PauseAnimation { duration: 3000 }
        NumberAnimation{
            duration:5000
            easing: Easing.InExpo
            target: applicationRoot
            property:"text_color_alpha"
            to:0
        }
    }

    state: "LoadingCamera"
    states: [
        State {
            name: "LoadingCamera"
            when: firstInit && camDevice.cameraStatus!=Camera.ActiveStatus
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_text
                text:"Loading Camera"
            }
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_progressbar
                value:0.25
            }
        },
        State {
            name: "LoadingStaticsModel"
            when: firstInit && camDevice.cameraStatus==Camera.ActiveStatus &&
                  staticsmodule.status!=Frame3DDKernel.LOADED
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_text
                text:"Loading 3D structure"
            }
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_progressbar
                value:0.5
            }
        },
        State {
            name: "Loading3DModel"
            when: firstInit && staticsmodule.status==Frame3DDKernel.LOADED &&
                  !scene3D.structureLoaded
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_text
                text:"Loading Scenario"
            }
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_progressbar
                value:0.75
            }

        },
        State {
            name: "LoadingVMManager"
            when: firstInit && scene3D.structureLoaded
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_text
                text:"Enjoy :)"
            }
            PropertyChanges {
                restoreEntryValues:false
                target: vmFrame3DDManager
                staticsModule:staticsmodule
            }
            PropertyChanges {
                restoreEntryValues:false
                target: vmFrame3DDManager
                sceneRoot:scene3D.structureEntity
            }
            PropertyChanges {
                restoreEntryValues:false
                target: loadingAnimation_progressbar
                value:1
            }
            StateChangeScript {
                script: {
                   closeLoadingAnimation.start()
                }
            }
        }
    ]

    Timer{
        id:closeLoadingAnimation
        onTriggered: {
            loadingAnimation.visible=false;
            loadingAnimation.enabled=false;
            firstInit=false;
        }
        running: false;
        interval: 2000
    }
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
                scene3D.structureLoaderURL=structure3DAsset;
            }
            else if(status==Frame3DDKernel.ERROR){
                loadingAnimation_text.text="Error";
                pageExit();
            }
        }
        onUpdated: text_animation.restart()
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
                 staticsmodule.sourceUrl=structureUrl;
             }
         }
         imageCapture.resolution: "640x480" //Android sets the viewfinder resolution as the capture one
         viewfinder.resolution:"640x480"
         focus {
                     focusMode: Camera.FocusContinuous
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
         filters:[aruco]
         fillMode:VideoOutput.PreserveAspectCrop
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
                 aspects: ["input","physics"]
                 //aspects:["input"]
                 multisample:true
                 DefaultScriptScene3D {
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

                }
                Item{
                    id:legend_item
                    anchors.bottom: parent.top
                    width: parent.width
                    height: parent.parent.border.width
                        Text{
                            width: parent.width/2
                            text:"Axial Forces:"
                            font.family: "Arial"
                            font.bold: true
                            color:"white"
                            font.pixelSize: legend_item.height/2
                            fontSizeMode: Text.Fit;
                            anchors.right: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            horizontalAlignment: Image.AlignRight
                            anchors.rightMargin: 5
                        }
                        Image{
                            id:compression_image
                            anchors.left: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            height: legend_item.height/3
                            fillMode: Image.PreserveAspectFit
                            source:"qrc:/images/Images/compression.png"
                            anchors.leftMargin: 20
                        }
                        Text{
                            id:compression_label
                            anchors.left: compression_image.right
                            anchors.verticalCenter: parent.verticalCenter
                            text:"Compression"
                            font.family: "Arial"
                            color:"white"
                            font.pixelSize: legend_item.height/3
                            fontSizeMode: Text.Fit;
                            anchors.leftMargin: 2
                        }
                        Image{
                            id:tension_image
                            anchors.left: compression_label.right
                            anchors.verticalCenter: parent.verticalCenter
                            height:  legend_item.height/3
                            fillMode: Image.PreserveAspectFit
                            source:"qrc:/images/Images/tension.png"
                            anchors.leftMargin: 10
                        }
                        Text{
                            anchors.left: tension_image.right
                            anchors.verticalCenter: parent.verticalCenter
                            text:"Tension"
                            font.family: "Arial"
                            color:"white"
                            font.pixelSize: legend_item.height/3
                            fontSizeMode: Text.Fit;
                            anchors.leftMargin: 2
                        }
                }

                Image {
                    id:ar_button
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
                Image {
                    id:ghostMode_button
                    anchors.bottom: parent.bottom
                    anchors.left: ar_button.right
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: scene3D.ghostMode?
                            "qrc:/icons/Icons/ghost_Mode_on.png" :
                            "qrc:/icons/Icons/ghost_Mode_OFF.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: scene3D.ghostMode = !scene3D.ghostMode
                    }
                }
                Image {
                    anchors.bottom: parent.bottom
                    anchors.left: ghostMode_button.right
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: scene3D.show_stress_ratio?
                                "qrc:/icons/Icons/show_stress_on.png" :
                                "qrc:/icons/Icons/show_stress_off.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: scene3D.show_stress_ratio = !scene3D.show_stress_ratio
                    }
                }
                Image {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.margins: 10
                    width: 50
                    height: 50
                    source: "qrc:/icons/Icons/back.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:pageExit()
                    }
                }
                Slider{
                    id:clippingPlaneSlider
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    orientation: Qt.Vertical
                    height: parent.height*0.3
                    value: 1000
                    stepSize: 100
                    minimumValue: 100
                    maximumValue: 3000
                    tickmarksEnabled: true

                }
                Rectangle{
                    anchors.left: clippingPlaneSlider.right
                    anchors.leftMargin: 15
                    anchors.top: clippingPlaneSlider.top
                    height: clippingPlaneSlider.height
                    Label{
                        text: "Depth Culling: "+clippingPlaneSlider.value/10+"cm"
                        color: "#F8F8F8"
                        rotation: 90
                        anchors.centerIn: parent
                    }
                }
             }
         }
     }
     ArucoDetector{
        id:aruco

        //pause: backgroundsubtraction.entropy<200
     }
     ArucoObject{
        id:structure_tag
        objectId: "Default"
        Component.onCompleted: {
            aruco.registerObserver(structure_tag)
        }
     }


     BackgroundSubtraction{
        id:backgroundsubtraction
        //onEntropyChanged: console.log(entropy)
     }

}
