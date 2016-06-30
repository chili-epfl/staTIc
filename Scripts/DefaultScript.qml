import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import ArucoDetector 1.0
import ArucoObject 1.0
import QuaternionHelper 1.0
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

//    property real text_color_alpha:0.0
//    SequentialAnimation{
//        id:text_animation
//        NumberAnimation{
//            duration:200
//            target: applicationRoot
//            property:"text_color_alpha"
//            from:0.0
//            to:1
//        }
//        PauseAnimation { duration: 3000 }
//        NumberAnimation{
//            duration:5000
//            target: applicationRoot
//            property:"text_color_alpha"
//            to:0
//        }
//    }

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
    Component.onCompleted: if(Platform=="ANDROID")
                               camDevice.deviceId=QtMultimedia.availableCameras[0].deviceId
                           else {
                               camDevice.deviceId=QtMultimedia.availableCameras[1].deviceId
                           }

    /*Loading animation*/
    Rectangle{
        id:loadingAnimation
        anchors.fill: parent
        color: "#2f3439"
        z:2
        Image {
            anchors.centerIn: parent
            id: loadingLogo
            width: parent.height/3
            height: width
            source:"qrc:/icons/Icons/LOADING.png"
        }
        Text{
           id:loadingAnimation_text
           anchors.horizontalCenter: loadingAnimation.horizontalCenter
           anchors.top: loadingLogo.bottom
           text: "Loading Camera"
           color: "white"
           font.pointSize: 15
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
        //onUpdated: text_animation.restart()
        onStartingUpdate:{
            scene3D.blinking_displacement=false;
            scene3D.blinking_stress=false;
        }
        onStabilityChanged: {
            if(stability==Frame3DDKernel.UNSTABLE){
                suggestion_box.text="The structure is unstable. Check the supports"
                suggestion_box_container.state="Visible"
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
                 staticsmodule.sourceUrl=structureUrl;
             }
         }

         //imageCapture.resolution: "1920x1440" //Android sets the viewfinder resolution as the capture one
         imageCapture.resolution: "640x480"
         viewfinder.resolution:"640x480"
//         imageCapture.resolution: "800x448" //Android sets the viewfinder resolution as the capture one
//         viewfinder.resolution:"800x448"
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
         fillMode: Image.PreserveAspectCrop
     }

     VideoOutput{
         z: 0
         id:viewFinder
         anchors.centerIn: parent
         anchors.fill: parent
         source: camDevice
         filters:[aruco]
         fillMode:VideoOutput.PreserveAspectCrop
         onContentRectChanged: if(camDevice.isRunning){
                                    scene3D.view_finder_width=viewFinder.contentRect.width
                                    scene3D.view_finder_height=viewFinder.contentRect.height
                               }
         Rectangle{
             anchors.fill: parent
             color:"#00000000"
             border.width: 0.125*(2*(parent.width+parent.height)-
                           Math.sqrt(4*(parent.width+parent.height)*(parent.width+parent.height)
                                     -16*(1-0.8)*(parent.width*parent.height))) //80% of screen
             border.color: "#80000000"

             Scene3D {
                 id:scene3DContainer
                 anchors.fill: parent
                 focus: true
                 aspects: ["input","physics"]
//                 aspects:["input"]
                 multisample:true
                 DefaultScriptScene3D {
                    id:scene3D
                     property real view_finder_width;
                     property real view_finder_height;
                     cameraScaleX: view_finder_height/scene3DContainer.height
                     cameraScaleY: view_finder_width/scene3DContainer.width
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
//                    width:Math.max(parent.width/3,pt2px(142))//5cm
                      width: parent.width/3;
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
                            id:tension_label
                            anchors.left: tension_image.right
                            anchors.verticalCenter: parent.verticalCenter
                            text:"Tension"
                            font.family: "Arial"
                            color:"white"
                            font.pixelSize: legend_item.height/3
                            fontSizeMode: Text.Fit;
                            anchors.leftMargin: 2
                        }
                        Image{
                            id:support_image
                            anchors.left: tension_label.right
                            anchors.verticalCenter: parent.verticalCenter
                            height:  legend_item.height/3
                            fillMode: Image.PreserveAspectFit
                            source:"qrc:/images/Images/support.png"
                            anchors.leftMargin: 10
                        }
                        Text{
                            anchors.left: support_image.right
                            anchors.verticalCenter: parent.verticalCenter
                            text:"Support"
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
                    id:show_stress_button
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
                        onClicked: {
                            scene3D.show_stress_ratio = !scene3D.show_stress_ratio
                            if(scene3D.show_displacement && scene3D.show_stress_ratio)
                                scene3D.show_displacement=false;
                            scene3D.blinking_stress=false;
                        }
                    }
                    Rectangle{
                        visible: scene3D.blinking_stress>0
                        width: parent.width
                        height: parent.height
                        radius: width/2
                        color: "transparent"
                        SequentialAnimation on color{
                            running: scene3D.blinking_stress>0
                            loops: -1
                            ColorAnimation {
                                to: scene3D.blinking_stress==1 ? Qt.rgba(1,0.64,0,0.8) : Qt.rgba(1,0,0,0.8)
                                duration: 1000
                            }
                            ColorAnimation {
                                to: scene3D.blinking_stress==1 ? Qt.rgba(1,0.64,0,0.2) : Qt.rgba(1,0,0,0.2)
                                duration: 1000
                            }
                        }
                   }

                }
                Image {
                    anchors.bottom: parent.bottom
                    anchors.left: show_stress_button.right
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: scene3D.show_displacement?
                                "qrc:/icons/Icons/show_displacement_on.png" :
                                "qrc:/icons/Icons/show_displacement_off.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            scene3D.show_displacement = !scene3D.show_displacement
                            if(scene3D.show_displacement && scene3D.show_stress_ratio)
                                scene3D.show_stress_ratio=false
                            scene3D.blinking_displacement=false;
                        }
                    }
                    Rectangle{
                        visible: scene3D.blinking_displacement>0
                        width: parent.width
                        height: parent.height
                        radius: width/2
                        color: "transparent"
                        SequentialAnimation on color{
                            running: scene3D.blinking_displacement>0
                            loops: -1
                            ColorAnimation {
                                to: scene3D.blinking_displacement==1 ? Qt.rgba(1,0.64,0,0.8) : Qt.rgba(1,0,0,0.8)
                                duration: 1000
                            }
                            ColorAnimation {
                                to: scene3D.blinking_displacement==1 ? Qt.rgba(1,0.64,0,0.2) : Qt.rgba(1,0,0,0.2)
                                duration: 1000
                            }
                        }
                   }
                }
                Rectangle{
                    id:suggestion_box_container
                    state:"Hidden"
                    states: [
                        State {
                            name: "Hidden"
                            AnchorChanges {
                                target: suggestion_box_container
                                anchors.top:parent.bottom
                            }
                            PropertyChanges {
                                target: suggestion_box_container
                                opacity:0
                            }
                        },
                        State {
                            name: "Visible"
                            AnchorChanges {
                                target: suggestion_box_container
                                anchors.bottom: parent.bottom
                            }
                            PropertyChanges {
                                target: suggestion_box_container
                                opacity:1
                            }
                            StateChangeScript{
                                script: {suggention_box_timer.restart()}
                            }
                        }
                    ]
                    transitions: Transition {
                        AnchorAnimation { duration: 500 }
                        NumberAnimation { property:"opacity" ; duration: 500 }
                    }
                    Timer{
                        id:suggention_box_timer
                        interval: 5000
                        running: false
                        onTriggered: suggestion_box_container.state="Hidden"
                    }
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.margins: 10
                    width: Math.min(suggestion_box.implicitWidth+20,parent.width/3)
                    height: Math.max(suggestion_box.implicitHeight+20,ghostMode_button.height)
                    radius: 5
                    border.width: 5
                    border.color: "#B4E1E4"
                    color: "#81c7e1"
                    Text{
                        id: suggestion_box
                        anchors.fill: parent
                        color: "white"
                        text:""
                        font.pointSize: 14
                        fontSizeMode: Text.Fit;
                        minimumPointSize: 10
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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
                    Text{
                        text: !clippingPlaneSlider.pressed ? "Depth Culling" : clippingPlaneSlider.value/10+"cm"
                        color: "#F8F8F8"
                        rotation: 90
                        anchors.centerIn: parent
                        fontSizeMode: Text.Fit
                        font.pointSize: 14

                    }
                }
             }
             Rectangle{
                visible: staticsmodule.stability==Frame3DDKernel.UNSTABLE
                anchors.fill: parent
                color: Qt.rgba(1,0,0,0.3)
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
