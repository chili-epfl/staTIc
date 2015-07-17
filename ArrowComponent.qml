import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity{
    property real myAngle:0;
    property real arrowLength:10;
    Mesh{
        id:tip_mesh
        source:"qrc:/icons/icons/arrow.obj"
    }
    Transform{
        id:tip_transform
        Rotate{
            axis:Qt.vector3d(0,1,0)
            angleRad: myAngle
        }
    }
    Entity{
        CylinderMesh{
            id:tail_mesh
            radius: 10
            length: arrowLength
        }
        Transform{
            id:tail_transform
            Rotate{
                axis:Qt.vector3d(0,0,1)
                angle: 90
            }
            Translate{
                dx:tail_mesh.length
            }
        }

        components: [tail_mesh,tail_transform]

    }
    components: [tip_mesh,tip_transform]
}

