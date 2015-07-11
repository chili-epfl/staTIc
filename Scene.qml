import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity {
    id: sceneRoot
    property alias sceneroot: sceneRoot
    Camera {
        id: camera
        projectionType: CameraLens.FrustumProjection
        nearPlane : 0.1
        farPlane : 1000

        top: 0.1*(chilitags.projectionMatrix.m23/chilitags.projectionMatrix.m11)
        bottom: -0.1*(chilitags.projectionMatrix.m23/chilitags.projectionMatrix.m11)
        left: -0.1*(chilitags.projectionMatrix.m13/chilitags.projectionMatrix.m22)
        right: 0.1*(chilitags.projectionMatrix.m13/chilitags.projectionMatrix.m22)

        position: Qt.vector3d( 0.0, 0.0, -1 )
        upVector: Qt.vector3d( 0.0, -1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
        onControlledCameraChanged: console.log(sphereTransform.matrix)

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

    PhongMaterial {
        id: material
    }


    Mesh {
        id: cuboidMesh
        source: "qrc:/models/models/untitled.obj"

    }

    Transform {
        id: sphereTransform

        Translate{dx:10
                  dy:10}
        MatrixTransform{matrix: tag.transform}

    }

    Entity {
        id: sphereEntity
        components: [ cuboidMesh, material, sphereTransform ]
    }

}
