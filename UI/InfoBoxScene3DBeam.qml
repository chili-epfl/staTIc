import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import DeformingBeamMesh 1.0
import "qrc:/opengl/Opengl/"

Entity {
    id: sceneRoot
    //property int zoom: 0
    property alias zoom: cameraController.zoom
    property bool resize: false

    property var segments: []
    property bool play: true
    property real beamLength
    property size beamSize
    property alias exagerate: deformingMesh.exagerate
    property alias panMode: cameraController.panMode

    onResizeChanged: {
        camera.position=Qt.vector3d( 0.0, 0.0, (0.8*(deformingMesh.length))/Math.tan(camera.fieldOfView/2) )
        camera.upVector= Qt.vector3d( 0.0, 1.0, 0.0 )
        camera.viewCenter= Qt.vector3d( 0.0, 0.0, 0.0 )
        resize=false;
    }

//    QQ2.NumberAnimation {
//        id:resizeAnimation
//        target: deformingMeshTransformation;
//        property: "scale";
//        duration: 500;
//        to: 1
//        running: false
//        alwaysRunToEnd: true
//    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 10000
        position: Qt.vector3d( 0.0, 0.0, (0.8*(deformingMesh.length))/Math.tan(fieldOfView/2) )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }


    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "#8cb5f5"
            }
        }
    ]

    CameraController{
        id:cameraController
        camera: camera
    }

    Entity{
        id:beamRelatedEntity
//        QQ2.NumberAnimation{
//            target:deformingMeshTransformation
//            properties: "scale"
//            duration: 5000
//            to: zoom > 0 ? 10 : -0.1
//            running: zoom!=0 ? true : false
//            alwaysRunToEnd: false
//        }

        DeformingBeamMesh{
            id:deformingMesh;
            segments: 50
            keyframes: 20
            displacements: sceneRoot.segments
            size: beamSize
            length: beamLength
            exagerate:1
        }
        Transform{
            id:deformingMeshTransformation
            rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 75)
            translation: Qt.vector3d(0,-deformingMesh.length/2,0)
            //matrix:syncPose
        }

        WireframeMaterial {
            id: deformingMeshMaterial
            effect: WireframeEffect {}
            ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
            diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )
            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: true

                QQ2.NumberAnimation {
                    target: deformingMeshMaterial;
                    property: "lineWidth";
                    duration: 1000;
                    from: 0.8
                    to: 1.8
                }

                QQ2.NumberAnimation {
                    target: deformingMeshMaterial;
                    property: "lineWidth";
                    duration: 1000;
                    from: 1.8
                    to: 0.8
                }

                QQ2.PauseAnimation { duration: 1500 }
            }

            QQ2.SequentialAnimation {
                loops: QQ2.Animation.Infinite
                running: play

                QQ2.NumberAnimation {
                    target: deformingMeshMaterial;
                    property: "state";
                    duration: 1000;
                    from: 0
                    to: deformingMesh.keyframes-1
                }

                QQ2.NumberAnimation {
                    target: deformingMeshMaterial;
                    property: "state";
                    duration: 1000;
                    from: deformingMesh.keyframes-1
                    to: 0
                }
            }
        }

        components: [deformingMesh,deformingMeshMaterial,deformingMeshTransformation]

        //Extreme planes
        PlaneMesh{
            id:planeMesh
            width: deformingMesh.size.height*1.5
            height: deformingMesh.size.width*1.5
        }
        PhongMaterial{
            id:planeMaterial
            ambient:"#ed6c44"
            diffuse:"black"
            specular:"black"
            shininess:0
        }

        Entity{
            property  var transform: Transform{
                rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
            }
            components: [
                planeMesh,
                transform,
                planeMaterial
            ]
        }
        Entity{
            property  var transform: Transform{
                rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
            }
            components: [
                planeMesh,
                transform,
                planeMaterial
            ]
        }
        Entity{
            property  var transform: Transform{
                translation:Qt.vector3d(deformingMesh.length,0,0)
                rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
            }
            components: [
                planeMesh,
                transform,
                planeMaterial
            ]
        }
        Entity{
            property  var transform: Transform{
                translation:Qt.vector3d(deformingMesh.length,0,0)
                rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
            }
            components: [
                planeMesh,
                transform,
                planeMaterial
            ]
        }

        //Extreme balls
        SphereMesh{
            id:extremeSphere
            radius: Math.min(deformingMesh.size.height,deformingMesh.size.width)/3
        }
        Entity{
            property var material: PhongMaterial{
                ambient:"#980000"
                diffuse:"black"
                specular:"black"
                shininess:0
            }
            property var transform: Transform{
                translation:Qt.vector3d(-extremeSphere.radius-0.05*deformingMesh.length,0,0)
            }
            components: [extremeSphere,material,transform]
        }
        Entity{
            property var material: PhongMaterial{
                ambient:"#479800"
                diffuse:"black"
                specular:"black"
                shininess:0
            }
            property var transform: Transform{
                translation:Qt.vector3d(extremeSphere.radius+(1.05)*deformingMesh.length,0,0)
            }
            components: [extremeSphere,material,transform]
        }
        Entity{
            property var material: PhongMaterial{
                ambient:"#001a98"
                diffuse:"black"
                specular:"black"
                shininess:0
            }
            property var transform: Transform{
                translation:Qt.vector3d(deformingMesh.length/2,extremeSphere.radius+(1.1)*deformingMesh.size.height/2,0)
            }
            components: [extremeSphere,material,transform]
        }

        Entity{
            components: [
                CylinderMesh{
                    radius: 0.1*extremeSphere.radius
                    length: (1.1)*deformingMesh.length
                },
                PhongMaterial{
                    ambient:"black"
                    diffuse:"black"
                },
                Transform{
                    rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                    translation:Qt.vector3d(deformingMesh.length/2,0,0)
                }
            ]
        }
    }
}
