import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity{
    property Mesh unitMesh
    property int unitId: 0
    property int step: 0
    property int animationValue: 0
    property int module: 1
    property int direction:1
    property bool rotate: false
    property real scaleFactor: 1
    Transform{
        id:transform
        Scale{
         scale: scaleFactor
        }
        Rotate{
            axis: Qt.vector3d(0,0,1)
            angle: rotate ? 180 : 0
        }
        Translate{
            dy:(((unitId)*step+animationValue)%module)*direction
        }
    }

    components: [
        transform,
        unitMesh
    ]
}

