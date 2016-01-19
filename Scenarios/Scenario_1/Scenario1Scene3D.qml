import Qt3D 2.0
import Qt3D.Renderer 2.0
import Qt3D.Input 2.0

import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0

import "qrc:/"

import "qrc:/tools/Tools/ConcentratedForce"
import "qrc:/tools/Tools/Scaffold"

Entity {
    id: sceneRoot

    property alias structureLoader : structureLoader
    property alias structureEntity: structureEntity

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

    Configuration  {
        controlledCamera: camera
    }

    MouseController {
        id: mouseController
    }

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
        objectName: "Structure"

        SceneLoader{
            id:structureLoader
            objectName: "structureLoader"
        }

        Transform {
            id: structureLoaderTransform
            Rotate{
                axis: Qt.vector3d(1, 0, 0)
                angle: -180
            }
            MatrixTransform{
                matrix: structure_tag.transform
            }
            Rotate{
                axis: Qt.vector3d(1, 0, 0)
                angle: 180
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
