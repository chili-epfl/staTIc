import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import QtQuick 2.3 as QQ2
import ARToolkit 1.0
import "qrc:/"

import "qrc:/ui/UI/"
import "qrc:/ui"
import "qrc:/element_views/Element_Views/"
Entity {
    id: sceneRoot


    /*Commons*/
    AnimationUnitDxCommons{
        id:audx_commons
    }
    BeamView_Commons{
        id:beam_commons
    }
    JointView_Commons{
        id:joint_commons
    }
    Char_Meshes{
        id:char_meshes
    }
    TrapezoidalForce_Commons{
        id:trapz_force_commons
    }
    /*.....*/

    property alias mouseEventHasBeenAccepted: deselection_guard.eventHasBeenAccepted

    property alias structureEntity: structureEntity

    property int globalNumericAnimation;
    onGlobalNumericAnimationChanged: console.log("Test")

    //scale factors to match the camera video with the 3D scene
    property real cameraScaleX:1;
    property real cameraScaleY:1;


    QQ2.NumberAnimation on globalNumericAnimation{
        duration: 10000
        from:0
        to:50
        running: false
        loops: QQ2.Animation.Infinite
    }


    Camera {
        id: scene_camera
        objectName: "camera"
        projectionType: CameraLens.FrustumProjection
        nearPlane : 0.1
        farPlane : 3000

        top: 0.1*(marker_detector.projectionMatrix.m23/(cameraScaleX*marker_detector.projectionMatrix.m11))
        bottom: -0.1*(marker_detector.projectionMatrix.m23/(cameraScaleX*marker_detector.projectionMatrix.m11))
        left: -0.1*(marker_detector.projectionMatrix.m13/(cameraScaleY*marker_detector.projectionMatrix.m22))
        right: 0.1*(marker_detector.projectionMatrix.m13/(cameraScaleY*marker_detector.projectionMatrix.m22))

        position: Qt.vector3d( 0.0, 0.0, 1 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [
        RenderSettings {
            pickingSettings.pickMethod: PickingSettings.TrianglePicking
            activeFrameGraph:
                RenderSurfaceSelector {
                Viewport {
                    id: viewport
                    objectName : "viewport"
                    normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)
                    // Use the specified camera
                    CameraSelector {
                        id : cameraSelector
                        objectName : "cameraSelector"
                        camera: scene_camera
                        SortPolicy {
                            sortTypes: [
                                SortPolicy.BackToFront
                            ]
                            ClearBuffers{
                                buffers: ClearBuffers.DepthBuffer
                                TechniqueFilter {
                                    matchAll: [ FilterKey { name: "renderingStyle"; value: "ar" } ]
                                }
                            }
                            ClearBuffers{
                                buffers : ClearBuffers.ColorBuffer
                                clearColor: "transparent"
                                TechniqueFilter {
                                    matchAll: [ FilterKey { name: "renderingStyle"; value: "forward" }]
                                }
                            }
                        }
                    }
                }
            }},
        InputSettings {}
    ]

    MouseDevice {
        id: mouseDevice
    }

    //If the user did not click on anything, the guard will clear the current selection
    Entity{
        id:deselection_guard
        property bool eventHasBeenAccepted:false;
        QQ2.Timer{
            id:guard_delay
            running: false
            interval: 200
            onTriggered:{
                if(!deselection_guard.eventHasBeenAccepted)
                    infobox.current_item=0;
                deselection_guard.eventHasBeenAccepted=false;
            }
        }
        MouseHandler{
            id:mouseInput
            sourceDevice: mouseDevice
            onReleased: {
                guard_delay.restart();
            }
        }
        components:[mouseInput]
    }


    Entity{
        Transform {
            id: structureLoaderTransform
            rotation: structure_tag.rotationQuaternion
            property quaternion inv_rotation: quaternion_helper.invert(rotation)
            property vector3d euler_angles: quaternion_helper.eulerAngles(rotation)
            QQ2.Timer{
                running: true
                repeat: true
                interval: 3000
                onTriggered: logger.log_position("Structure",structure_tag.translation,structureLoaderTransform.rotationX,structureLoaderTransform.rotationY,structureLoaderTransform.rotationZ)
            }
            translation: structure_tag.has_appeared ? structure_tag.translation : staticsModule.initialPose
            QQ2.Component.onCompleted:
                structure_tag.appendQuaternion(fromAxisAndAngle(1,0,0,90))
        }
        components: [structureLoaderTransform]


        Entity {
            //The actual scene root which is the parent of all the joints, beams, loads entities
            id:structureEntity
            function findEntityByName(name){
                for(var child_index=0;child_index<childNodes.length;child_index++){
                    if(childNodes[child_index])
                        if(childNodes[child_index].objectName===name)
                            return childNodes[child_index];
                }
            }
            function findBeams(){
                var beams=[]
                for(var child_index=0;child_index<childNodes.length;child_index++){
                    if(childNodes[child_index])
                        if(childNodes[child_index].type==="beam")
                            beams.push(childNodes[child_index]);
                }
                return beams;
            }
            property vector3d eulerAngles
            property vector3d translation:Qt.vector3d(10,0,0)
            components: [
                Transform{
                    rotation: fromEulerAngles(parent.eulerAngles)
                    translation: parent.translation.times(staticsModule.modelScale)
                }
            ]
        }
    }



}
