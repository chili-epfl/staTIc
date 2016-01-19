import Qt3D.Core 2.0
import Qt3D.Render 2.0
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
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(position)
            return m;
        }
    }
    PhysicsBodyInfo{
        id:body
        kinematic:true
        inputTransform: transform
    }
    components:[mesh,body,transform]
}

