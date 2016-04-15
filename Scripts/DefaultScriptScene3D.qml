import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0

import "qrc:/"

import "qrc:/tools/Tools/ConcentratedForce"
import "qrc:/tools/Tools/Scaffold"
import "qrc:/tools/Tools/TangibleTrapzLoad"
import "qrc:/tools/Tools/Snow"

import "qrc:/ui/UI/"

Entity {
    id: sceneRoot

    property url structureLoaderURL
    onStructureLoaderURLChanged: {
       if(structureLoaderURL.toString().search(".dae")!=-1){
           structureLoader.source=structureLoaderURL;
           structureLoaded=Qt.binding(function(){return structureLoader.status==SceneLoader.Loaded})
       }
       else if(structureLoaderURL.toString().search(".obj")!=-1){
           meshStructure.source=structureLoaderURL;
           structureLoaded=true;
       }
    }
    property bool structureLoaded: false
    property alias structureEntity: structureEntity

    property bool ghostMode: false
    property int globalNumericAnimation;
    QQ2.NumberAnimation on globalNumericAnimation{
        duration: 10000
        from:0
        to:50
        running: true
        loops: QQ2.Animation.Infinite
    }
    Camera {
        id: camera
        objectName: "camera"
        projectionType: CameraLens.FrustumProjection
        nearPlane : 0.1
        farPlane : clippingPlaneSlider.value

        top: 0.1*(aruco.projectionMatrix.m23/aruco.projectionMatrix.m11)
        bottom: -0.1*(aruco.projectionMatrix.m23/aruco.projectionMatrix.m11)
        left: -0.1*(aruco.projectionMatrix.m13/aruco.projectionMatrix.m22)
        right: 0.1*(aruco.projectionMatrix.m13/aruco.projectionMatrix.m22)

        position: Qt.vector3d( 0.0, 0.0, 0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, -1.0 )
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
        FrameGraph {
            activeFrameGraph:
                ForwardRenderer {
                id: viewport
                clearColor: "transparent"
                camera:camera

            }

        },
        PhysicsWorldInfo{
            gravity: Qt.vector3d(0,0,0);
            scaleFactor: 1
        }

    ]


    Entity {
        id:structureEntity
        Transform {
            id: structureLoaderTransform
//            translation:Qt.vector3d(0,-100,-1000)
            rotation:structure_tag.rotationQuaternion
            translation:structure_tag.translation
            QQ2.Component.onCompleted:
                structure_tag.appendQuaternion(fromAxisAndAngle(1,1,1,120))
        }

        components: [structureLoaderTransform]

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



        /*Tools*/

//        ConcentratedForce{

//        }

        /*Scaffold{

        }*/
//        Snow{
//        }
    }

//    TangibleTrapzLoad{

//    }



}
