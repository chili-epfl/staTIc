import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0
import ARToolkit 1.0
import "qrc:/"

import "qrc:/tools/Tools/ConcentratedForce"
import "qrc:/tools/Tools/Scaffold"
import "qrc:/tools/Tools/TangibleTrapzLoad"
import "qrc:/tools/Tools/Snow"

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
    Runtime_Meshes{
        id:runtime_meshes
    }
    TrapezoidalForce_Commons{
        id:trapz_force_commons
    }
    /*.....*/

    property alias mouseEventHasBeenAccepted: deselection_guard.eventHasBeenAccepted
    property url structureLoaderURL
    onStructureLoaderURLChanged: {
        if(structureLoaderURL.toString().search(".dae")!=-1){
            structureLoader.source=structureLoaderURL;
            structureLoaded=Qt.binding(function(){return structureLoader.status==SceneLoader.Loaded})
            structureLoaded=true
        }
        else if(structureLoaderURL.toString().search(".obj")!=-1){
            meshStructure.source=structureLoaderURL;
            structureLoaded=true;
        }
    }
    property bool structureLoaded: true
    property alias structureEntity: structureEntity
    property alias scene_camera: scene_camera
    property bool ghostMode: false

    property int globalNumericAnimation;

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
        farPlane : clippingPlaneSlider.value

        top: 0.1*(marker_detector.projectionMatrix.m23/(cameraScaleX*marker_detector.projectionMatrix.m11))
        bottom: -0.1*(marker_detector.projectionMatrix.m23/(cameraScaleX*marker_detector.projectionMatrix.m11))
        left: -0.1*(marker_detector.projectionMatrix.m13/(cameraScaleY*marker_detector.projectionMatrix.m22))
        right: 0.1*(marker_detector.projectionMatrix.m13/(cameraScaleY*marker_detector.projectionMatrix.m22))

        position: Qt.vector3d( 0.0, 0.0, 1 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    //    Configuration  {
    //        controlledCamera: camera
    //    }

    //    MouseController {
    //        id: mouseController
    //    }



    //    CameraController{
    //        camera: camera
    //    }

    components: [
        RenderSettings {
            activeFrameGraph:TechniqueFilter {
                objectName : "techniqueFilter"

                // Select the forward rendering Technique of any used Effect
                matchAll: [ FilterKey { name: "renderingStyle"; value: "forward" } ]
                RenderSurfaceSelector {
                    // Use the whole viewport
                    Viewport {
                        id: viewport
                        objectName : "viewport"
                        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)
                        // Use the specified camera
                        CameraSelector {
                            id : cameraSelector
                            objectName : "cameraSelector"
                            camera: scene_camera
                            ClearBuffers {
                                buffers : ClearBuffers.ColorDepthBuffer
                                clearColor: "transparent"
//                                SortPolicy {
//                                    sortTypes: [
////                                        SortPolicy.BackToFront,
//                                        SortPolicy.StateChangeCost,
//                                        SortPolicy.Material
//                                    ]
//                                }
                            }
                        }
                    }
                }
            }

        },
        PhysicsWorldInfo{
            gravity: Qt.vector3d(0,0,0);
            scaleFactor: 1
        },
        InputSettings {}
    ]

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
        //       MouseInput{
        //           id:mouseInput
        //           controller: mouseController
        //           onReleased: {
        //               guard_delay.restart();
        //           }
        //       }
        //       components:[mouseInput]
    }


    Entity{

        components: [Transform{
                translation:Qt.vector3d(0,0,-1000)
            },
            CuboidMesh{xExtent: 100;yExtent: 100;zExtent: 100},
            PhongMaterial{}
        ]

    }

    Entity{
        components:[
            Transform {
                rotation:  !structure_tag.objectIsVisible ?
                               quaternion_helper.product(
                                   quaternion_helper.product(
                                       fromAxisAndAngle(0,1,0,-(rotationSensor.reading.z-
                                                                structure_tag.last_sensor_read.z)),
                                       fromAxisAndAngle(0,0,-1,-(rotationSensor.reading.x-
                                                                 structure_tag.last_sensor_read.x))),
                                   fromAxisAndAngle(-1,0,0,-(rotationSensor.reading.y-
                                                             structure_tag.last_sensor_read.y)))
                             :
                               Qt.quaternion(1,0,0,0)
            }
        ]

        Entity{
            Transform {
                id: structureLoaderTransform
                //            translation:Qt.vector3d(0,-100,-1000)
                rotation: structure_tag.rotationQuaternion
                property quaternion inv_rotation: quaternion_helper.invert(rotation)
                property vector3d euler_angles: quaternion_helper.eulerAngles(rotation)
                QQ2.Timer{
                    running: true
                    repeat: true
                    interval: 3000
                    onTriggered: logger.log_position("Structure",structure_tag.translation,structureLoaderTransform.rotationX,structureLoaderTransform.rotationY,structureLoaderTransform.rotationZ)
                }
                //            QuaternionAnimation on rotation{
                //            }
                // translation:settings.focus_on_joint ? Qt.vector3d(0,0,-300) : structure_tag.translation
                translation: structure_tag.translation

                QQ2.Component.onCompleted:
                    //structure_tag.appendQuaternion(fromAxisAndAngle(1,1,1,120))
                    structure_tag.appendQuaternion(fromAxisAndAngle(1,0,0,90))
            }
            components: [structureLoaderTransform]


            Entity {
                id:structureEntity

                components: [
                    Transform{
                        //                    translation:settings.focus_on_joint ?
                        //                                    infobox.current_item.position.times(-1)
                        //                                  :
                        //                                    Qt.vector3d(0,0,0)
                    }
                ]


                Entity{
                    enabled:ghostMode
                    Mesh{
                        id:meshStructure
                    }
                    SceneLoader{
                        id:structureLoader
                        objectName: "structureLoader"
                    }
                    PhongAlphaMaterial {
                        id:material
                        ambient:  "#783e0b"
                        diffuse:"grey"
                        specular:"black"
                        alpha:0.80
                    }
                    components: structureLoader.status==SceneLoader.Loaded?
                                    [structureLoader]:
                                    [meshStructure, material]


                }

                Scaffold{
                    extreme1_tag: ARToolkitObject{
                        objectId: "Mat_240"
                        QQ2.Component.onCompleted: marker_detector.registerObserver(this)
                    }
                    extreme2_tag: ARToolkitObject{
                        objectId: "Mat_241"
                        QQ2.Component.onCompleted: marker_detector.registerObserver(this)
                    }
                }

                //        Scaffold{
                //            extreme1_tag: ArucoObject{
                //                objectId: "242"
                //                QQ2.Component.onCompleted: aruco.registerObserver(this)
                //            }
                //            extreme2_tag: ArucoObject{
                //                objectId: "243"
                //                QQ2.Component.onCompleted: aruco.registerObserver(this)
                //            }
                //        }
                /*Tools*/

                //        ConcentratedForce{

                //        }

                /*Scaffold{

        }*/
                //        Snow{
                //        }

            }
        }
    }



    //    TangibleTrapzLoad{

    //    }



}
