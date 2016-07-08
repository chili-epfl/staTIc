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
import "qrc:/"
import Warehouse3D 1.0
Item{

    id: applicationRoot
    anchors.fill: parent
    signal pageExit();

    property alias currentViewFilter: viewFilterBar.selection
    property alias materialsManager: staticsmodule.materialsManager

    property alias maxForce : staticsmodule.maxForce
    property alias minForce : staticsmodule.minForce

    property url structureUrl;
    property url structure3DAsset;
    property url structureTagConfig;

    property alias settings: settings
    property alias infobox: infobox

    property bool firstInit : true
    property bool stateLock: false
    QuaternionHelper{
        id:quaternion_helper
    }

    Settings{
        id:settings
    }

    state: "LoadingCamera"

    states: [
        //....Init states....
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
        },//....end....//
        State {
            when:!firstInit && stateLock
            name: "custom"
        },
        State{
            name: ""
            StateChangeScript{
                script: settings.restore_defaults();
            }
        }
    ]

    Item{
        states: [
            State {
                name: "beam"
                when: !firstInit && settings.show_filter_bar && currentViewFilter=='ANALYSIS' &&
                      infobox.current_item!=0 && infobox.current_item.type==="beam"
                      && applicationRoot.state==""
                PropertyChanges {
                    target: infobox.loader
                    source:"qrc:/ui/UI/InfoBoxBeam.qml"
                    restoreEntryValues: false
                }
                PropertyChanges {
                    target: infobox
                    lateral_visibility:"Visible"
                }
                PropertyChanges {
                    target:settings
                    show_spatial_references: true
                    show_info_box: true
                    load_is_selectable:false
                }
            },
            State {
                name: "joint"
                when: !firstInit &&  settings.show_filter_bar && currentViewFilter=='ANALYSIS' &&
                      infobox.current_item!=0 && infobox.current_item.type==="joint"
                      && applicationRoot.state==""
                PropertyChanges {
                    target: infobox.loader
                    source:"qrc:/ui/UI/InfoBoxJoint.qml"
                    restoreEntryValues: false
                }
                PropertyChanges {
                    target: infobox
                    lateral_visibility:"Visible"
                }
                PropertyChanges {
                    target:settings
                    show_spatial_references: true
                    show_info_box: true
                    load_is_selectable:false
                }
            },
            State {
                name: "designer"
                when: !firstInit && settings.show_filter_bar && currentViewFilter=='DESIGNER'
                      && applicationRoot.state==""
                PropertyChanges {
                    target: infobox.loader
                    source:"qrc:/ui/UI/InfoBoxDesigner.qml"
                    restoreEntryValues: false
                }
                PropertyChanges {
                    target: infobox
                    lateral_visibility:"Visible"
                }
                PropertyChanges {
                    target: settings
                    show_spatial_references: false
                    show_info_box:true
                    load_is_selectable:true
                }
            }
        ]


    }


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
            settings.blink_stress=0;
            settings.blink_displacement=0;
        }
        onStabilityChanged: {
            if(stability==Frame3DDKernel.UNSTABLE){
                suggestion_box.text="The structure is unstable. Check the supports"
            }
        }
    }


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

         imageCapture.resolution: "640x480"
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
                    width: parent.width/3;
                }

                Item{
                    id:legend_item
                    visible: settings.show_legend
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
                    visible: settings.show_stress_button
                    anchors.bottom: parent.bottom
                    anchors.left: ghostMode_button.right
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: settings.show_stress?
                                "qrc:/icons/Icons/show_stress_on.png" :
                                "qrc:/icons/Icons/show_stress_off.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            settings.show_stress = !settings.show_stress
                            if(settings.show_displacement && settings.show_stress)
                                settings.show_displacement=false;
                            settings.blink_stress=0;
                        }
                    }
                    Rectangle{
                        visible: settings.blink_stress>0
                        width: parent.width
                        height: parent.height
                        radius: width/2
                        color: "transparent"
                        SequentialAnimation on color{
                            running: settings.blink_stress>0
                            loops: -1
                            ColorAnimation {
                                to: settings.blink_stress==1 ? Qt.rgba(1,0.64,0,0.8) : Qt.rgba(1,0,0,0.8)
                                duration: 1000
                            }
                            ColorAnimation {
                                to: settings.blink_stress==1 ? Qt.rgba(1,0.64,0,0.2) : Qt.rgba(1,0,0,0.2)
                                duration: 1000
                            }
                        }
                   }

                }

                Image {
                    visible: settings.show_displacement_button
                    anchors.bottom: parent.bottom
                    anchors.left: show_stress_button.right
                    anchors.margins: 10
                    width: 100
                    height: 100
                    source: settings.show_displacement?
                                "qrc:/icons/Icons/show_displacement_on.png" :
                                "qrc:/icons/Icons/show_displacement_off.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            settings.show_displacement = !settings.show_displacement
                            if(settings.show_displacement && settings.show_stress)
                                settings.show_stress=false
                            settings.blink_displacement=0;
                        }
                    }
                    Rectangle{
                        visible: settings.blink_displacement>0
                        width: parent.width
                        height: parent.height
                        radius: width/2
                        color: "transparent"
                        SequentialAnimation on color{
                            running: settings.blink_displacement>0
                            loops: -1
                            ColorAnimation {
                                to: settings.blink_displacement==1 ? Qt.rgba(1,0.64,0,0.8) : Qt.rgba(1,0,0,0.8)
                                duration: 1000
                            }
                            ColorAnimation {
                                to: settings.blink_displacement==1 ? Qt.rgba(1,0.64,0,0.2) : Qt.rgba(1,0,0,0.2)
                                duration: 1000
                            }
                        }
                   }
                }

                SuggestionBox{
                    id:suggestion_box
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
