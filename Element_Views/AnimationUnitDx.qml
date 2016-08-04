import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity{
    property Mesh unitMesh
    property PhongMaterial material:{
        if(isReaction)
            return audx_commons.material_reaction;
        else if(rotate){
            return audx_commons.material_240_360;
        }
        else if(!rotate)
            return audx_commons.material_0_360;
    }

    property int unitId: 0
    property int step: 0
    property int animationValue: 0
    property int module: 1
    property int direction:1
    property bool rotate:false
    property real scaleFactor: 1
    property bool isReaction: false
    property int offset: isReaction ? -2:2
    Transform{
        id:transform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(Qt.vector3d((((unitId)*step+animationValue)%module+offset)*direction
                                    , 0, 0));
            if(rotate)
                m.rotate(90, Qt.vector3d(0, 0, 1))
            else
                m.rotate(-90, Qt.vector3d(0, 0, 1))
            m.scale(scaleFactor)

            return m;
        }
    }

    components: [
        transform,
        unitMesh,material
    ]
}

