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
            m.scale(scaleFactor)
            if(rotate)
                m.rotate(90, Qt.vector3d(0, 0, 1))
            else
                m.rotate(-90, Qt.vector3d(0, 0, 1))

            m.translate(Qt.vector3d((((unitId)*step+animationValue)%module)*direction
                                    , 0, 0));
            return m;
        }
    }
    GoochMaterial { id: gooch
        diffuse: Qt.rgba( 1.0, 0.75, 1.0, 1.0 )
        specular: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        alpha: 0.2
        beta: 0.6
    }
    components: [
        transform,
        unitMesh,gooch
    ]
}

