import Qt3D 2.0
import Qt3D.Renderer 2.0
import MaterialSetter 1.0
import QtQuick 2.3
import "qrc:/opengl/opengl"

Entity {
    id: sceneRoot
    property alias sceneroot: sceneRoot
    property alias camera: camera

    property alias sceneloader : sceneloader
    property alias sceneloadermatrix:sceneloadermatrix

    Camera {
        id: camera
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
        onStatusChanged: {if (sceneloader.status===SceneLoader.Loaded){
                eventhandler.sceneRoot=sceneroot;
            }}
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

    MaterialSetter{
        id:materialsetter
        sceneroot:scene
        entityName: "Scene"
        material: transparentMaterial
        onMaterialChanged: {if(sceneloader.status==SceneLoader.Loaded) materialsetter.onAnyChange();}
    }
    Connections{
        target: sceneloader
        onStatusChanged:{ if(sceneloader.status==SceneLoader.Loaded) materialsetter.onAnyChange(); }
    }

    Entity {
        id:scene
        objectName: "Model"
        components: [ sceneloader, sceneloadertransform ]
    }


}
