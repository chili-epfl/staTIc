import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "trapezoidalForce"
    property vector3d globalForce: Qt.vector3d(0,-weight*9.8,0);

    property url asset3DMeshURL;
    property url asset3DTextureURL;
    property real weight
    property vector2d extent

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
    property vector3d relativeLocalPosition:rootEntity.parent?
                                                Qt.vector3d(-transform.translation.y/rootEntity.parent.length + 0.5,-transform.translation.y/rootEntity.parent.length + 0.5,-transform.translation.y/rootEntity.parent.length + 0.5):
                                                Qt.vector3d(0,0,0);
    onRelativeLocalPositionChanged: {
        if(dragging){
            var pos_text="("+relativeLocalPosition.x+","+relativeLocalPosition.y+","+relativeLocalPosition.z+")"
            logger.log("Trapezoidal_force_move", {"relative_position":pos_text,"beam":parent.objectName})
        }
    }

    property bool dragging: settings.beam_dragging_ownership==rootEntity;
    property vector3d offsetAugmentation: rootEntity.parent? Qt.vector3d(rootEntity.parent.tangibleSection.height/2,0,
                                                                         -rootEntity.parent.tangibleSection.width/2):
                                                             Qt.vector3d(0,0,0)
    onDraggingChanged: {
        if(dragging && rootEntity.parent!=null){
            transform.translation=Qt.binding(function(){
                    return offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
            })
        }
        else
            transform.translation=offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
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


    property Mesh customMesh: runtime_meshes.getMesh(asset3DMeshURL)
    property DiffuseMapMaterial material_diffuse: runtime_meshes.getTexture(asset3DTextureURL)

    property Material material: isSelected || dragging ? trapz_force_commons.material_for_selection :material_diffuse

    property bool isSelected: infobox.current_item==rootEntity

    onIsSelectedChanged: {
        if(!isSelected)
            rootEntity.parent.drag_anchor_enabled=false
    }

    ObjectPicker {
        id:valid_picker
        hoverEnabled: false
        onPressedChanged: if(pressed && settings.beam_dragging_ownership==0){
                              if(settings.load_is_selectable && infobox.current_item!=rootEntity)
                                  infobox.current_item=rootEntity
                              if(settings.load_is_draggable){
                                  settings.beam_dragging_ownership=rootEntity
                                  rootEntity.parent.drag_anchor_enabled=true;
                                  rootEntity.parent.current_anchor_position=transform.translation.minus(offsetAugmentation)
                                  resetTimer.restart()
                              }
                          }
                          else if(!pressed && settings.beam_dragging_ownership==rootEntity){
                              sceneRoot.mouseEventHasBeenAccepted=true;
                              rootEntity.parent.drag_anchor_enabled=false;
                              settings.beam_dragging_ownership=0;
                              transform.translation=offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
                          }
        onClicked: {
            sceneRoot.mouseEventHasBeenAccepted=true;
            if(settings.load_is_selectable && infobox.current_item!=rootEntity)
                    infobox.current_item=rootEntity
            if(dragging){
                rootEntity.parent.drag_anchor_enabled=false;
                settings.beam_dragging_ownership=0
                transform.translation=offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
            }
        }
    }


    components: [transform,customMesh,material,valid_picker]


    QQ2.Timer{
        id:resetTimer
        interval: 2000
        onTriggered: {
            if(dragging){
                rootEntity.parent.drag_anchor_enabled=false;
                settings.beam_dragging_ownership=0
                transform.translation=offsetAugmentation.plus(rootEntity.parent.current_anchor_position);
            }
        }
        running: false;
    }

}
