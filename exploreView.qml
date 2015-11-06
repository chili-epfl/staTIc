import QtQuick 2.0
import QtQuick.Controls 1.3
import QtMultimedia 5.5
import QtQuick.Layouts 1.1
import QtQuick.Scene3D 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0

import Frame3DDKernel 1.0
import Frame3DDVMManager 1.0

import MouseInterface3D 1.0

Item{
    id: applicationRoot
    anchors.fill: parent
    property alias currentViewFilter: viewFilterBar.selection
    signal exitView()
    signal loadStructure(string modelName,url modelFile,url staticsFile,url tagFile);

    onLoadStructure: {
        staticsmodule.sourceUrl=staticsFile //<-------Should start later...
        scene.sceneloader.source=modelFile
        chilitags.tagConfigurationFile=tagFile
        tag.name=modelName
    }

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
            }
        }
    ]

    Frame3DDKernel{
        id:staticsmodule
        onStatusChanged: {
            if(status===Frame3DDKernel.LOADED){
                vmFrame3DDManager.staticsModule=staticsmodule
            }
        }
    }

    Frame3DDVMManager{
            id:vmFrame3DDManager
    }

    /*UI*/
    Item{
        id:topmenu
        anchors.horizontalCenter: parent.horizontalCenter
        y:0 ; z:1
        state: "HIDDEN"
        ColumnLayout{
            Row{
                ViewFilterBar{
                    id:viewFilterBar
                }
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
                PropertyChanges { target: topmenu; y: -viewFilterBar.height+10}
                PropertyChanges { target: ic_topmenu; rotation:180}

            }
        ]
        Behavior on y {animation: NumberAnimation{duration: 250}}
    }

    Rectangle{
        id:detailsview
        objectName: "Details_Tab"
        color: "transparent"
        anchors.verticalCenter: parent.verticalCenter
        x:applicationRoot.width-width
        z:1
        width: 0.25*parent.width
        height: Math.min(0.5*parent.height,detailedViewJoint.count*0.3*width + 5)
        state: detailedViewJoint.count>0 ? "DetailsJoint" : "Closed"
        states: [
            State {
                name: "DetailsJoint"
                PropertyChanges {
                    target: detailsview
                    x: applicationRoot.width-0.25*parent.width

                }
            },
            State {
                name: "DetailsBeam"
                PropertyChanges {
                    target: detailsview
                    x: applicationRoot.width-0.25*parent.width
                }
            },
            State {
                name: "Closed"
                PropertyChanges {
                    target: detailsview
                    x: applicationRoot.width-5
                }
            }

        ]
        Behavior on x {animation: NumberAnimation{duration: 250}}

        DetailedViewJoint{
            objectName: "Details_Tab_Joints"
            id:detailedViewJoint
            onModelChanged: console.log(model)
        }


    }


    Item{
        id:leftmenu
        z:1
        anchors.verticalCenter: parent.verticalCenter
        ColumnLayout{
            spacing: 0
            Rectangle{
                color: (applicationRoot.state=="AG")? "red": "yellow"
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
                        applicationRoot.state="AG"
                    }
                }
            }
            Rectangle{
                color: (applicationRoot.state=="V")? "red" : "yellow"
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
                        applicationRoot.state="V"
                    }
                }
            }
        }
    }

    Connections{
        target: mousearea
        onCustomClicked:{
            eventhandler.inputEventHandler(EventHandler2D.CLICKED,{"CurrentTool":toolspace.state,"Point":p0,"Entity":entity});
        }
        onCustomDragged:{
            eventhandler.inputEventHandler(EventHandler2D.DRAGGED,{"CurrentTool":toolspace.state,"Point0":p0,"Entity0":p0_entity,
                                                    "Point1":p1,"Entity1":p1_entity});
        }
        onCustomHeld:{
            eventhandler.inputEventHandler(EventHandler2D.HELD,{"CurrentTool":toolspace.state,"Point":p0,"Entity":entity,
                                                    "Timespan":timespan});
        }
        onCustomHolding:{
            eventhandler.inputEventHandler(EventHandler2D.HOLDING,{"CurrentTool":toolspace.state,"Point0":p0,"Entity0":p0_entity,
                                                    "Point1":p1,"Entity1":p1_entity,"Timespan":timespan});
        }

    }


    /*3D Rendering*/

    Camera{
        id:camDevice
        deviceId:QtMultimedia.availableCameras[1].deviceId
        imageCapture.resolution: "640x480" //Android sets the viewfinder resolution as the capture one
        viewfinder.resolution:"640x480"
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
            aspects: ["input","physics"]
            multisample:true
            Scene {
                id:scene
            }
        }
        MouseInterface3D{
            id:mouseInterface
            scene3D:scene.sceneroot
            camera:scene.camera

        }
        MouseArea{
            id:mousearea
            anchors.fill: parent
            property bool fastRelease: true
            property vector2d p0_normalized;
            property variant p0_entity;
            property vector2d p1_normalized;
            property variant p1_entity;
            property int timespan;

            signal customClicked (vector2d p0,variant entity);
            signal customDragged (vector2d p0,variant p0_entity,vector2d p1,variant p1_entity);
            signal customHeld (vector2d p0,variant entity,int timespan);
            signal customHolding (vector2d p0,variant p0_entity,vector2d p1,variant p1_entity,int timespan);

            Timer {
                id:fastReleaseTimer
                interval: 100; running: false; repeat: false
                onTriggered: parent.fastRelease=false
            }
            Timer {
                id:holdingTimer
                interval: 100; running: false; repeat: true
                onTriggered: {
                    parent.timespan+=interval
                    var p_tmp=Qt.vector2d((2.0 * parent.mouseX)/parent.width - 1.0,1.0-(2.0 * parent.mouseY)/parent.height);
                    var p_tmp_entity=mouseInterface.select(p_tmp);
                    parent.customHolding(parent.p0_normalized,parent.p0_entity,p_tmp,p_tmp_entity,parent.timespan);
                }
            }
            state:"IDLE"
            states:[
                State {
                    name: "IDLE"
                    PropertyChanges {
                        target:mousearea
                        onPressed:{
                            fastRelease=true;
                            fastReleaseTimer.start();
                            timespan=0;
                            holdingTimer.start();
                            p0_normalized=Qt.vector2d((2.0 * mouseX)/width - 1.0,1.0-(2.0 * mouseY)/height);
                            p0_entity=mouseInterface.select(p0_normalized);
                            state="PRESSED";
                        }
                        onReleased:{}
                    }
                },
                State {
                    name: "PRESSED"
                    PropertyChanges {
                        target:mousearea
                        onPressed:{}
                        onReleased:{
                            fastReleaseTimer.stop();
                            holdingTimer.stop();
                            p1_normalized= Qt.vector2d((2.0 * mouseX)/width - 1.0,1.0-(2.0 * mouseY)/height);
                            p1_entity=mouseInterface.select(p1_normalized);
                            var diff=p0_normalized.minus(p1_normalized);
                            if(diff.length()>0.10){
                                customDragged(p0_normalized,p0_entity,p1_normalized,p1_entity);
                            }
                            else{
                                if(fastRelease){
                                    customClicked(p0_normalized,p0_entity);
                                }
                                else{
                                    customHeld(p0_normalized,p0_entity,timespan);
                                }

                            }
                            state="IDLE";
                        }
                    }
                }
            ]
        }
    }

    Chilitags{
        id:chilitags
        chiliobjects: [tag]
    }

    ChilitagsObject{
        id: tag
    }





}
