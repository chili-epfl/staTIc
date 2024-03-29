import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import ARToolkit 1.0
import QuaternionHelper 1.0
import Qt3D.Core 2.0 as Q3D
import Frame3DDKernel 1.0
import "qrc:/ui/UI/"
import "qrc:/"
import Warehouse3D 1.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
Item{

    id: applicationRoot
    anchors.fill: parent
    signal pageExit();

    property alias marker_detector: marker_detector
    property alias materialsManager: staticsmodule.materialsManager
    property alias staticsModule: staticsmodule
    property alias warehouse: warehouse3d
    property alias sceneRoot: scene3D.structureEntity

    property alias maxForce : staticsmodule.maxForce
    property alias minForce : staticsmodule.minForce

    property url structureUrl;
    property url structure3DAsset;

    property alias settings: settings
    property alias infobox: infobox

    property bool firstInit : true
    property bool stateLock: false

    property alias suggestion_box: suggestion_box


    QuaternionHelper{
        id:quaternion_helper
    }

    Settings{
        id:settings
    }

    state: "LoadingCamera"
    //    onStateChanged: console.log("State:"+state)
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
        },

        //....end....//
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


    Timer{
        id:closeLoadingAnimation
        onTriggered: {
            loadingAnimation.visible=false;
            loadingAnimation.enabled=false;
            firstInit=false;
            //logger.log("Start_default_script",{"structureUrl":structureUrl})
        }
        running: false;
        interval: 2000
    }

    Component.onCompleted: if(Qt.platform.os=="android"){
                               camDevice.deviceId=QtMultimedia.availableCameras[0].deviceId
                           }
                           else  {
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
            indeterminate: true
        }
    }
    /************/

    Frame3DDKernel{
        id:staticsmodule
        sceneRoot: scene3D.structureEntity
        gravity:Qt.vector3d(0,0,0)
        onStatusChanged: {
            if(firstInit && status==Frame3DDKernel.LOADED){
                loadingAnimation_text.text="Enjoy :)"
                closeLoadingAnimation.start()
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
                suggestion_box.show_alert_message("The analysis cannot be performed. The structure seeems unstable. Check the supports, the beam properties or reduce the loads.")
            }
        }
//        onUpdated: {
//            logger.log("Static_module_update",{"stability":stability})
//        }

    }


    SoundEffect {
        id: crashSound

        source: settings.enable_sounds ? "qrc:/soundeffects/AR/SoundEffects/crash_wood_2.wav" : ""
    }
    Connections{
        target: settings
        onBlink_stressChanged: if(settings.blink_stress==2 && !crashSound.playing){
                                   creekSound.stop()
                                   crashSound.play()
                               }
    }
    SoundEffect {
        id: creekSound
        source: settings.enable_sounds ? "qrc:/soundeffects/AR/SoundEffects/creak_low_2.wav" : ""

    }


    Warehouse3D{
        id:warehouse3d
    }

    /*3D Rendering*/
    Camera{
        id:camDevice
        viewfinder.resolution: "640x480"
//        viewfinder.onResolutionChanged: console.log(viewfinder.resolution)
//        Component.onCompleted: {
//            var preferred_aspect_ratio=Qt.platform.os=="android" ? 1.7:
//                                                             Math.floor(10*applicationRoot.width/applicationRoot.height)/10
//            var supportedResolutions=supportedViewfinderResolutions();
//            var found_good_candidate=false
//            var good_candidate="640x480";

//            for(var i=0;i<supportedResolutions.length;i++){
//                var aspect_ratio=Math.floor(10*supportedResolutions[i].width/supportedResolutions[i].height)/10;
//                console.log(aspect_ratio)
//                console.log(supportedResolutions[i].width," ",supportedResolutions[i].height)
//                if(aspect_ratio==preferred_aspect_ratio && supportedResolutions[i].width>=640 && supportedResolutions[i].width<1500 ){
//                    found_good_candidate=true
//                    good_candidate=supportedResolutions[i];
//                    console.log("got")
//                }
//            }
//            viewfinder.resolution=Qt.size(good_candidate.width,good_candidate.height)
//            console.log(viewfinder.resolution)
//        }
        onCameraStatusChanged: {
            if(firstInit && camDevice.cameraStatus==Camera.ActiveStatus ){
                loadingAnimation_text.text="Loading 3D structure";
                staticsmodule.sourceUrl=structureUrl;

            }
        }
        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceAuto
        focus.focusMode: CameraFocus.FocusContinuous
        focus.focusPointMode: CameraFocus.FocusPointAuto
        captureMode: Camera.CaptureStillImage
        imageCapture {
            resolution: viewfinder.resolution
            onImageCaptured: {
                stillImage.source = preview
                camDevice.stop();
            }
        }
        property bool isRunning: true
        onIsRunningChanged: {
            if(isRunning){
                camDevice.start();
            }
            else{
                camDevice.imageCapture.capture();
                //camDevice.stop();
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
        filters:[marker_detector]
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
                aspects:["input"]
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

                RowLayout{
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 10
                    spacing: 10

                    Image {
                        id:ar_button
                        visible: settings.show_AR_button
                        Layout.preferredWidth:  100
                        Layout.preferredHeight: 100
                        source: camDevice.isRunning ?
                                    "qrc:/icons/Icons/AR_ON.png" :
                                    "qrc:/icons/Icons/AR_OFF.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                camDevice.isRunning = !camDevice.isRunning
                                //logger.log("AR_Button_Click",{"running":camDevice.isRunning})
                            }
                        }
                    }

                    Image {
                        id:show_stress_button
                        visible: settings.show_stress_button
                        Layout.preferredWidth:  100
                        Layout.preferredHeight: 100
                        source: settings.show_stress?
                                    "qrc:/icons/Icons/show_stress_on.png" :
                                    "qrc:/icons/Icons/show_stress_off.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                settings.show_stress = !settings.show_stress
                                //                            if(settings.show_displacement && settings.show_stress)
                                //                                settings.show_displacement=false;
                                settings.blink_stress=0;
                                //logger.log("Show_Stress_Click",{"visible":settings.show_stress})
                            }
                        }

                        Rectangle{
                            visible: settings.show_stress
                            anchors.bottom: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: stress_col.implicitWidth+10
                            height: stress_col.implicitHeight+10
                            border.width: 2
                            radius:2
                            border.color:Qt.platform.os=="android" ? "white" :"black"
                            color: Qt.platform.os=="android" ? "#2f3439" : "white"
                            Column{
                                id:stress_col
                                anchors.centerIn: parent
                                ExclusiveGroup { id: stressGroup }
                                RadioButton {
                                    text: "Axial"
                                    checked: true
                                    onCheckedChanged: if(checked) settings.stress_type=1
                                    exclusiveGroup: stressGroup
                                }
                                RadioButton {
                                    text: "Bending"
                                    onCheckedChanged: if(checked) settings.stress_type=2
                                    exclusiveGroup: stressGroup
                                }
                                RadioButton {
                                    text: "Shear"
                                    onCheckedChanged: if(checked) settings.stress_type=3
                                    exclusiveGroup: stressGroup
                                }
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
                        Layout.preferredWidth:  100
                        Layout.preferredHeight: 100
                        source: settings.show_displacement?
                                    "qrc:/icons/Icons/show_displacement_on.png" :
                                    "qrc:/icons/Icons/show_displacement_off.png"
                        MouseArea{
                            id:show_disp_button
                            anchors.fill: parent
                            onClicked: {
                                settings.show_displacement = !settings.show_displacement
                                //                            if(settings.show_displacement && settings.show_stress)
                                //                                settings.show_stress=false
                                settings.blink_displacement=0;
                                if(settings.show_displacement)
                                    exagerate_disp_slider.visible=true
                                else
                                    exagerate_disp_slider.visible=false
                                //logger.log("Show_Displacement_Click",{"visible":settings.show_displacement,"exagerate":exagerate_disp_slider.value})
                            }
                            hoverEnabled: true
                            onHoveredChanged:if(settings.show_displacement)
                                                 exagerate_disp_slider.visible=true
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
                        Slider{
                            id:exagerate_disp_slider
                            visible:false
                            width: parent.width*2
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.top
                            anchors.margins: 10
                            stepSize: 1
                            minimumValue: 1
                            tickmarksEnabled: true
                            maximumValue: 25
                            value: 1
//                            onValueChanged: {
//                                logger.log("Show_Displacement_Click",{"visible":settings.show_displacement,"exagerate":exagerate_disp_slider.value})
//                            }
                            Binding{
                                target:settings
                                property: "exagerate_displacement_factor"
                                value: if(exagerate_disp_slider.value<10){
                                           return exagerate_disp_slider.value;
                                       }
                                       else if(exagerate_disp_slider.value<20){
                                           return (exagerate_disp_slider.value-9)*10
                                       }
                                       else {
                                           return 100+(exagerate_disp_slider.value%20)*50;
                                       }


                            }
                            Text{
                                text:"Exagerate: "+settings.exagerate_displacement_factor+"x"
                                fontSizeMode: Text.Fit
                                horizontalAlignment: Text.AlignHCenter
                                color: "#F8F8F8"
                                font.pointSize: 14
                                width: parent.width
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.bottom: parent.top
                            }

                        }

                    }
                }


                Image {
                    visible: settings.show_back_button
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.margins: 10
                    width: 50
                    height: 50
                    source: "qrc:/icons/Icons/back.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{
                            staticsModule.storePoseOffsetAndModelScale(sceneRoot.translation,sceneRoot.eulerAngles)
                            pageExit()
                        }
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

    ARToolkit{
        id:marker_detector
        matrixCode: ARToolkit.MATRIX_CODE_4x4_BCH_13_9_3
        defaultMarkerSize: 35
        labelingThreshold: 150
        flip_image: Qt.platform.os=="android"
        Component.onCompleted: {
            if(applicationWindow.board_path=="")
                loadMultiMarkersConfigFile("default","qrc:/AR/board_configuration.dat")
            else
                loadMultiMarkersConfigFile("default","file:"+applicationWindow.board_path)
        }

    }
    ARToolkitObject{
        id:structure_tag
        objectId: "default"
        property bool has_appeared:false
        onObjectIsVisibleChanged:{
            if(objectIsVisible)
                has_appeared=true
        }

        Component.onCompleted: {
            marker_detector.registerObserver(structure_tag)
        }
    }

    SuggestionBox{
        id:suggestion_box


    }



}
