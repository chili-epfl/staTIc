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
    property bool isReaction: false
    Transform{
        id:transform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d((((unitId)*step+animationValue)%module)*direction
                                    , 0, 0));
            if(rotate)
                m.rotate(90, Qt.vector3d(0, 0, 1))
            else
                m.rotate(-90, Qt.vector3d(0, 0, 1))
            m.scale(scaleFactor)

            return m;
        }
    }

    PhongMaterial{
        id:material
        property real h: {
            if(isReaction)
                return 240/360;
            else if(rotate)
                return 120/360
            else if(!rotate)
                return 0/360
        }
        ambient:Qt.hsla(h,1,0.5)
        diffuse:"grey"
        specular:"black"
        shininess:0
    }

    components: [
        transform,
        unitMesh,material
    ]
}

