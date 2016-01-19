import Qt3D.Core 2.0
import Qt3D.Renderer 2.0
import MaterialSetter 1.0
import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0
import PhysicsSetter 1.0

import "qrc:/opengl/opengl"
import "qrc:/ConcentratedForce"
import "qrc:/Scaffold"

Entity {
    id: sceneRoot
    QQ2.Component.onCompleted: {
        console.log(applicationRoot.currentViewFilter)
    }
    property alias sceneroot: sceneRoot
    property alias camera: camera

    property alias sceneloader : sceneloader
    property alias sceneloadermatrix:sceneloadermatrix

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

    SceneLoader{
        id:sceneloader
        objectName: "sceneloader"
    }
    QQ2.Connections{
        target: sceneloader
        onStatusChanged:{
            if(sceneloader.status==SceneLoader.Loaded){
                vmFrame3DDManager.sceneRoot=modelEntity;
                //physicsSetter.onAnyChange();
                //materialsetter.onAnyChange();
            }
        }
    }

    Transform {
        id: sceneloadertransform
        Rotate{
            axis: Qt.vector3d(1, 0, 0)
            angle: -180
        }
        MatrixTransform{
            id: sceneloadermatrix
            matrix: tag.transform
        }
        Rotate{
            axis: Qt.vector3d(1, 0, 0)
            angle: 180
        }
    }

    TransparentMaterial {
        id: transparentMaterial
    }

    /*MaterialSetter{
        id:materialsetter
        sceneroot:scene
        entityName: "Scene"
        material: transparentMaterial
        onMaterialChanged: {if(sceneloader.status==SceneLoader.Loaded) materialsetter.onAnyChange();}
    }*/
    /*PhysicsBodyInfo{
        id:bodyInfoScene
        kinematic: true
        //mass:1
    }
    PhysicsSetter{
        id:physicsSetter
        sceneroot: scene
        entityName: "Scene"
        bodyInfo: bodyInfoScene
    }*/



    Entity {
        id:modelEntity
        objectName: "Model"
        components: [ sceneloader, sceneloadertransform ]

//        SphereMesh{
//            id:debugMesh
//            radius: 20
//        }
//        //components: [ debugMesh, sceneloadertransform ]

        /*Tools*/
        ConcentratedForce{

        }

        Scaffold{

        }
    }




}
