import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "trapezoidalForce"
    property vector3d globalForce: Qt.vector3d(0,-weight*9800,0);

    property url asset3DMeshURL;
    property url asset3DTextureURL;
    property real weight

    signal killMe();

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
    property vector2d extent: Qt.vector2d(-5,5)

    property bool dragging: false;
    property vector3d offsetAugmentation: Qt.vector3d(rootEntity.parent.tangibleSection.height/2,0,
                                                      -rootEntity.parent.tangibleSection.width/2)
    onDraggingChanged: {
        if(dragging && rootEntity.parent!=null){
            transform.translation=Qt.binding(function(){
                return offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
            })
        }
        else
            transform.translation=transform.translation;
    }
    /*Visual aspect*/
    Transform{
        id:transform
        rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
        translation:offsetAugmentation
        onTranslationChanged:{
                    resetTimer.restart()
        }
    }
    property ObjectPicker objectPicker: ObjectPicker {
        hoverEnabled: false
        onClicked: {
                if(applicationRoot.currentViewFilter=='DESIGNER')
                    infobox.current_item=rootEntity
                rootEntity.parent.drag_anchor_enabled=false;
        }
        onPressed: {
            if(applicationRoot.currentViewFilter=='DESIGNER')
                infobox.current_item=rootEntity
            rootEntity.parent.drag_anchor_enabled=true;
            rootEntity.parent.current_anchor_position=transform.translation.minus(offsetAugmentation)
            dragging=true;
        }
        onReleased: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
    }

    Mesh{
        id:customMesh
        source: asset3DMeshURL
    }
    DiffuseMapMaterial {
        id:material_diffuse
        ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        diffuse:  asset3DTextureURL
        shininess: 2.0
    }
    PhongAlphaMaterial{
        id:material_selection
        ambient:  "yellow"
        diffuse:"grey"
        specular:"black"
        alpha:0.80
    }
    property Material material: isSelected ? material_selection:material_diffuse
    property bool isSelected: infobox.current_item==rootEntity
    onIsSelectedChanged: {
        if(!isSelected)
            rootEntity.parent.drag_anchor_enabled=false
    }
    components: [transform,customMesh,material,objectPicker]


    QQ2.Timer{
        id:resetTimer
        interval: 2000
        onTriggered: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
        running: false;
    }
}
