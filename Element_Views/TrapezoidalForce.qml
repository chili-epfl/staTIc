import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "trapezoidalForce"
    property vector3d globalForce: Qt.vector3d(0,-980000,0);

//    property vector3d localForce: globalToLocalMat.times(globalForce);
//    onLocalForceChanged: console.log(localForce);
//    property matrix4x4 globalToLocalMat;
//    function computeGlobalToLocal(){
//        var a=Qt.vector3d(1,0,0);
//        var b=rootEntity.parent.extreme2.minus(rootEntity.parent.extreme1).normalized();
//        var axb=a.crossProduct(b);
//        var result=Qt.matrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
//        if(axb.x!==0 || axb.y!==0 || axb.z!==0){
//            var tmp_ssc=ssc(axb);
//            result=result.plus(tmp_ssc).plus(tmp_ssc.times(tmp_ssc).times((1-a.dotProduct(b))/Math.pow(axb.length(),2)));
//            result.m44=1;
//        }else{
//            if(b.x===-1){
//                result.m11=-1;
//                result.m22=-1;
//            }
//        }
//        return result.inverted();
//    }

    /*Force is acting in the y direction*/
    property vector3d relativeLocalPosition:Qt.vector3d(0,-transform.translation.y/rootEntity.parent.length + 0.5,0)
    property vector2d extent: Qt.vector2d(5,5)

    property bool dragging: false;
    /*Visual aspect*/
    CuboidMesh{
        id:mesh
        xExtent: 10
        yExtent: 10
        zExtent: 10
    }

    property ObjectPicker objectPicker: ObjectPicker {
        hoverEnabled: false
        onClicked: {
                infobox.current_item=rootEntity
                rootEntity.parent.drag_anchor_enabled=false;
        }
        onPressed: {
            infobox.current_item=rootEntity
            rootEntity.parent.drag_anchor_enabled=true;
            rootEntity.parent.current_anchor_position=transform.translation
            dragging=true;
        }
        onReleased: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
    }
    Transform{
        id:transform
        scale3D:Qt.vector3d(2,2,2)
        QQ2.Binding on translation{
            when: dragging && rootEntity.parent!=null
            value:rootEntity.parent.current_anchor_position
        }
        onTranslationChanged:{
                    resetTimer.restart()
        }
    }

    components: [mesh,objectPicker,transform]
    QQ2.Timer{
        id:resetTimer
        interval: 3000
        onTriggered: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
        running: false;
    }
}
