import Qt3D.Core 2.0
import Qt3D.Render 2.0

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
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d(0, (((unitId)*step+animationValue)%module)*direction, 0));
            if(rotate)
                m.rotate(180, Qt.vector3d(0, 0, 1))
            m.scale(scaleFactor)
            return m;
        }
    }

    components: [
        transform,
        unitMesh
    ]
}

