import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtPhysics.unofficial 1.0


Entity {
    objectName: "Anchor"
    property vector3d position
    SphereMesh{
        enabled: false
        id:mesh
        radius: 20
    }
    Transform{
        id: transform
        Translate{
            translation: position
        }
    }
    PhysicsBodyInfo{
        id:body
        kinematic:true
        inputTransform: transform
    }
    components:[mesh,body,transform]
}

