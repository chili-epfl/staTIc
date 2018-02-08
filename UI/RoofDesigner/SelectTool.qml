import QtQuick 2.7

Item{
    property var current_target:undefined;

    function onPressed(target,mouse){
        if(target.class_type=="Sketch" ){
            target.pinch_area.pinch.target=target
            target.mouse_area.drag.target=target
            current_target=target;
        }
    }

    function onReleased(target,mouse){
        if(target.class_type=="Sketch" ){
            target.pinch_area.pinch.target=undefined
            target.mouse_area.drag.target=undefined
            current_target=undefined;
        }
    }

    function abort(){
        if(current_target!==undefined){
            current_target.pinch_area.pinch.target=undefined
            current_target.mouse_area.drag.target=undefined
        }
        current_target=undefined
    }

    function onClicked(target,mouse){
        if(target.class_type=="Point") {
            target.showContextMenu(mouse.x,mouse.y)
        }

        if(target.class_type=="Point" || target.class_type=="Line"){
            var data = {"object": target}
            var removed = false
            for(var i = 0; i < constraintsPanel.listEntities.count; i++){
                if (constraintsPanel.listEntities.get(i).object == target){
                    constraintsPanel.listEntities.remove(i)
                    removed = true
                    break
                }
            }
            if (!removed){
                constraintsPanel.listEntities.append(data)
            }
            constraintsVisibility()
        }else{
//            constraintsPanel.listEntities.clear()
//            constraintsVisibility()
        }
    }

    function constraintsVisibility(){
        var pointCount = 0
        var lineCount = 0
        for(var i = 0; i < constraintsPanel.listEntities.count; i++){
            switch (constraintsPanel.listEntities.get(i).object.class_type){
            case "Point":
                pointCount++
                break
            case "Line":
                lineCount++
                break
            default:
                break
            }
        }
        if (pointCount == 0 && lineCount > 0){
            constraintsPanel.horz_const.enabled = true
            constraintsPanel.vert_const.enabled = true
            constraintsPanel.leng_const.enabled = true
            constraintsPanel.dist_const.enabled = false
            if (lineCount > 1) {
                constraintsPanel.equL_const.enabled = true
                constraintsPanel.para_const.enabled = true
                if (lineCount == 2){
                    constraintsPanel.perp_const.enabled = true
                    constraintsPanel.angl_const.enabled = true
                } else {
                    constraintsPanel.perp_const.enabled = false
                    constraintsPanel.angl_const.enabled = false
                }
            } else {
                constraintsPanel.equL_const.enabled = false
                constraintsPanel.para_const.enabled = false
                constraintsPanel.perp_const.enabled = false
                constraintsPanel.angl_const.enabled = false
            }
            constraintsPanel.midP_const.enabled = false
        } else if (pointCount == 1 && lineCount == 1){
            constraintsPanel.horz_const.enabled = false
            constraintsPanel.vert_const.enabled = false
            constraintsPanel.leng_const.enabled = false
            constraintsPanel.equL_const.enabled = false
            constraintsPanel.dist_const.enabled = false
            constraintsPanel.para_const.enabled = false
            constraintsPanel.perp_const.enabled = false
            constraintsPanel.angl_const.enabled = false
            constraintsPanel.midP_const.enabled = true
        }  else if (pointCount == 2 && lineCount == 0){
            constraintsPanel.horz_const.enabled = false
            constraintsPanel.vert_const.enabled = false
            constraintsPanel.leng_const.enabled = false
            constraintsPanel.equL_const.enabled = false
            constraintsPanel.dist_const.enabled = true
            constraintsPanel.para_const.enabled = false
            constraintsPanel.perp_const.enabled = false
            constraintsPanel.angl_const.enabled = false
        } else {
            constraintsPanel.horz_const.enabled = false
            constraintsPanel.vert_const.enabled = false
            constraintsPanel.leng_const.enabled = false
            constraintsPanel.equL_const.enabled = false
            constraintsPanel.dist_const.enabled = false
            constraintsPanel.para_const.enabled = false
            constraintsPanel.perp_const.enabled = false
            constraintsPanel.angl_const.enabled = false
            constraintsPanel.midP_const.enabled = false
        }
    }
}
