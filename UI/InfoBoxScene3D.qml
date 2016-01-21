import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import DeformingBeamMesh 1.0
import "qrc:/opengl/Opengl/"
Entity {
    id: sceneRoot
    property var segments: []
    property matrix4x4 syncPose
    property bool play: true
    property alias deformableMeshLength: deformingMesh.length
    property alias deformableMeshSize: deformingMesh.size

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, (0.7*(deformingMesh.length+10)+0.3*0.7*(deformingMesh.length+10))/Math.tan(fieldOfView/2) )
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
        camera: camera
    }

    Entity{
        DeformingBeamMesh{
            id:deformingMesh;
            segments: 50
            keyframes: 20
            displacements: sceneRoot.segments
            length: 60
            exagerate:exagerateSlider.value
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
            width: deformingMesh.size.width*2
            height: deformingMesh.size.height*2
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
            radius: 2
        }
        Entity{
            property var material: PhongMaterial{
                ambient:"#980000"
                diffuse:"black"
                specular:"black"
                shininess:0
            }
            property var transform: Transform{
                translation:Qt.vector3d(-10,0,0)
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
                translation:Qt.vector3d(deformingMesh.length+10,0,0)
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
                translation:Qt.vector3d(deformingMesh.length/2,10,0)
            }
            components: [extremeSphere,material,transform]
        }

        Entity{
            components: [
                CylinderMesh{
                    radius: 0.3
                    length: deformingMesh.length+20
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
