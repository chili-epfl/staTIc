import Qt3D 2.0
import Qt3D.Renderer 2.0
import MaterialSetter 1.0
import QtQuick 2.3 as QQ2
import QtPhysics.unofficial 1.0
import PhysicsSetter 1.0

import "qrc:/opengl/opengl"
import "qrc:/ConcentratedForce"

Entity {
    id: sceneRoot
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
        /*Translate{
           dz: 100
        }*/
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
    PhysicsBodyInfo{
        id:bodyInfoScene
        kinematic: true
        //mass:1
    }
    PhysicsSetter{
        id:physicsSetter
        sceneroot: scene
        entityName: "Model"
        bodyInfo: bodyInfoScene
    }

    QQ2.Connections{
        target: sceneloader
        onStatusChanged:{
            if(sceneloader.status==SceneLoader.Loaded){
                physicsSetter.onAnyChange();
                eventhandler.sceneRoot=sceneroot;
                //materialsetter.onAnyChange();
            }
        }

    }
    SphereMesh{
        id:debugMesh
        radius: 20
    }

    Entity {
        id:scene
        objectName: "Model"
        components: [ sceneloader, sceneloadertransform ]
        //components: [ debugMesh, sceneloadertransform ]

    }

    ConcentratedForce{
        id:forceTool
    }




}
