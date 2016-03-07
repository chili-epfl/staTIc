import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0

import "qrc:/"

import "qrc:/tools/Tools/ConcentratedForce"
import "qrc:/tools/Tools/Scaffold"
import "qrc:/ui/UI/"
Entity {
    id: sceneRoot

    property alias structureLoader : structureLoader
    property alias structureEntity: structureEntity
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
        farPlane : 100000

        top: 0.1*(chilitags.projectionMatrix.m23/chilitags.projectionMatrix.m11)
        bottom: -0.1*(chilitags.projectionMatrix.m23/chilitags.projectionMatrix.m11)
        left: -0.1*(chilitags.projectionMatrix.m13/chilitags.projectionMatrix.m22)
        right: 0.1*(chilitags.projectionMatrix.m13/chilitags.projectionMatrix.m22)

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

        }

    ]


    Entity {
        id:structureEntity

        SceneLoader{
            id:structureLoader
            objectName: "structureLoader"
        }

        Transform {
            id: structureLoaderTransform
            matrix:{
                var m=Qt.matrix4x4()
                m.rotate(-180, Qt.vector3d(1, 0, 0))
                m=m.times(structure_tag.transform)
                m.rotate(180, Qt.vector3d(1, 0, 0))
                return m;
            }
        }

        components: [ structureLoaderTransform ]


        /*Tools*/
        ConcentratedForce{

        }

        /*Scaffold{

        }*/





    }



}
