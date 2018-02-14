import QtQuick 2.7
import QtQuick.Window 2.0
import Constraints 1.0
import QtQuick.Controls 2.2

Item {

    anchors.fill:parent

    property real scaleFactor: Screen.pixelDensity
    property var origin;

    property real zoomFactor: 1
    property int zoom_origin_x: width/2
    property int zoom_origin_y: height/2

    property int index_lines:0
    property int index_points: 0
    property int index_constraints: 0

    property url background_picture_url: ""

    transformOrigin: Item.TopLeft
    transform: Scale{
        origin.x:zoom_origin_x
        origin.y:zoom_origin_y
        xScale: zoomFactor
        yScale: zoomFactor
    }




    property alias mouse_area: mouse_area
    property alias pinch_area: pinch_area

    property alias constraints: constraints

    readonly property string class_type: "Sketch"

    property var undo_buffer: []
    property var redo_buffer: []

    signal undo()
    signal redo()

    onUndo: {
        if(undo_buffer.length>0){
            redo_buffer.push(undo_buffer.pop())
            if(undo_buffer.length>0){

            }else{

            }
        }
    }

    onRedo: {
        if(redo_buffer.length>0){
            var state=redo_buffer.pop()
            undo_buffer.push(state)
        }
    }

    signal store_state(var epoch)
    onStore_state: {
        var state={}
        undo_buffer.push(state)
        redo_buffer=[]
    }

    PinchArea{
        id:pinch_area
        anchors.fill: parent
        onPinchUpdated: {
            if(pinch.scale>0){
                var new_scale=Math.max(1, parent.zoomFactor*(pinch.scale/pinch.previousScale));
                parent.zoomFactor=new_scale;
            }
            parent.zoom_origin_x=pinch.center.x
            parent.zoom_origin_y=pinch.center.y
        }

        MouseArea{
            id:mouse_area
            anchors.fill: parent
            drag.smoothed: false
            preventStealing:true
            scrollGestureEnabled:false
            hoverEnabled:  Qt.platform.os!="android"
            enabled: true
            onPressed: {
                current_tool.onPressed(parent.parent,mouse);
            }
            onReleased: {
                current_tool.onReleased(parent.parent,mouse);
            }
            onClicked: {
                current_tool.onClicked(parent.parent,mouse)
            }
            onCanceled: current_tool.abort()
            onWheel: {
                //parent.parent.x=wheel.x-parent.parent.width/2
                var new_scale
                if(wheel.angleDelta.y>0){
                    new_scale=parent.parent.zoomFactor+0.10
                    parent.parent.zoomFactor=new_scale
                    parent.parent.zoom_origin_x=mouse_area.mouseX
                    parent.parent.zoom_origin_y=mouse_area.mouseY

                }
                else if(wheel.angleDelta.y<0){
                    new_scale=Math.max(1,parent.parent.zoomFactor-0.10)
                    parent.parent.zoomFactor=new_scale
                    parent.parent.zoom_origin_x=mouse_area.mouseX
                    parent.parent.zoom_origin_y=mouse_area.mouseY
                }
            }
        }
    }

    Constraints {
        id: constraints

        property var constraint_component

        Component.onCompleted:  {
            constraint_component = Qt.createComponent("Constraint.qml");
            constraints.apply(sketch)
        }

        //property ListModel list: ListModel {}

        function add(){
            if (constraintsPanel.horz_const.enabled && constraintsPanel.horz_const.checked) {
                for(var e = 0; e < constraintsPanel.listEntities.count; e++){
                    var c = constraint_component.createObject(sketch)
                    c.type = 0
                    c.entityA = constraintsPanel.listEntities.get(e).object
                }
            }
            if (constraintsPanel.horz_const.enabled && constraintsPanel.vert_const.checked) {
                for(e = 0; e < constraintsPanel.listEntities.count; e++){
                    c = constraint_component.createObject(sketch)
                    c.type = 1
                    c.entityA = constraintsPanel.listEntities.get(e).object
                }
            }
            if (constraintsPanel.leng_const.enabled && constraintsPanel.leng_const.checked) {
                for(e = 0; e < constraintsPanel.listEntities.count; e++){
                    var oldExists = false
                    for (var i = 0; i < sketch.children.length && !oldExists; i++) {
                        if (sketch.children[i].class_type == "Constraint" &&
                                sketch.children[i].existing) {
                            c = sketch.children[i]
                            if (c.type === 2 &&
                                    (c.ptA === constraintsPanel.listEntities.get(e).object.p1 &&
                                     c.ptB === constraintsPanel.listEntities.get(e).object.p2) ||
                                    (c.ptA === constraintsPanel.listEntities.get(e).object.p2 &&
                                     c.ptB === constraintsPanel.listEntities.get(e).object.p1)){
                                oldExists = true
                                c.valA = constraintsPanel.leng_const.value
                                updateScaleFactor(c)
                            }
                        }
                    }
                    if (!oldExists){
                        c = constraint_component.createObject(sketch)
                        c.type = 2
                        c.valA = constraintsPanel.leng_const.value
                        c.ptA = constraintsPanel.listEntities.get(e).object.p1
                        c.ptB = constraintsPanel.listEntities.get(e).object.p2
                        updateScaleFactor(c)
                    }
                }
            }
            if (constraintsPanel.equL_const.enabled && constraintsPanel.equL_const.checked) {
                for(e = 1; e < constraintsPanel.listEntities.count; e++){
                    c = constraint_component.createObject(sketch)
                    c.type = 3
                    c.entityA = constraintsPanel.listEntities.get(0).object
                    c.entityB = constraintsPanel.listEntities.get(e).object
                }
            }
            if (constraintsPanel.dist_const.enabled && constraintsPanel.dist_const.checked) {
                oldExists = false
                for (i = 0; i < sketch.children.length && !oldExists; i++) {
                    if (sketch.children[i].class_type == "Constraint" &&
                            sketch.children[i].existing) {
                        c = sketch.children[i]
                        if (c.type === 4 &&
                                (c.ptA === constraintsPanel.listEntities.get(0).object &&
                                 c.ptB === constraintsPanel.listEntities.get(1).object) ||
                                (c.ptA === constraintsPanel.listEntities.get(1).object &&
                                 c.ptB === constraintsPanel.listEntities.get(0).object)){
                            oldExists = true
                            c.valA = constraintsPanel.dist_const.value
                            updateScaleFactor(c)
                        }
                    }
                }
                if (!oldExists){
                    c = constraint_component.createObject(sketch)
                    c.type = 4
                    c.valA = constraintsPanel.dist_const.value
                    c.ptA = constraintsPanel.listEntities.get(0).object
                    c.ptB = constraintsPanel.listEntities.get(1).object
                    updateScaleFactor(c)
                }
            }
            if (constraintsPanel.para_const.enabled && constraintsPanel.para_const.checked) {
                for(e = 1; e < constraintsPanel.listEntities.count; e++){
                    c = constraint_component.createObject(sketch)
                    c.type = 5
                    c.entityA = constraintsPanel.listEntities.get(0).object
                    c.entityB = constraintsPanel.listEntities.get(e).object
                }
            }
            if (constraintsPanel.perp_const.enabled && constraintsPanel.perp_const.checked) {
                c = constraint_component.createObject(sketch)
                c.type = 6
                c.entityA = constraintsPanel.listEntities.get(0).object
                c.entityB = constraintsPanel.listEntities.get(1).object
            }
            if (constraintsPanel.angl_const.enabled && constraintsPanel.angl_const.checked) {
                oldExists = false
                for (i = 0; i < sketch.children.length && !oldExists; i++) {
                    if (sketch.children[i].class_type == "Constraint" &&
                            sketch.children[i].existing) {
                        c = sketch.children[i]
                        if(c.type === 7 &&
                                ((c.entityA === constraintsPanel.listEntities.get(0).object &&
                                  c.entityB === constraintsPanel.listEntities.get(1).object) ||
                                 (c.entityA === constraintsPanel.listEntities.get(1).object &&
                                  c.entityB === constraintsPanel.listEntities.get(0).object))){
                            oldExists = true
                            c.valA = 180 - constraintsPanel.angl_const.value
                        }
                    }
                }
                if (!oldExists){
                    c = constraint_component.createObject(sketch)
                    c.type = 7
                    c.valA = 180 - constraintsPanel.angl_const.value
                    c.entityA = constraintsPanel.listEntities.get(0).object
                    c.entityB = constraintsPanel.listEntities.get(1).object
                }
            }

            //if (constraintsPanel.midP_const.enabled && constraintsPanel.midP_const.checked) {
            //                var pId = constraintsPanel.listEntities.get(0).object.class_type == "Point" ? 0 : 1
            //                c = constraint_component.createObject(sketch)
            //                c.type = 8
            //                c.ptA = constraintsPanel.listEntities.get(pId).object
            //                c.entityA = constraintsPanel.listEntities.get(1 - pId).object
            //}
        }

        function updateScaleFactor(c) {
            var d = c.ptA.distance(c.ptB)
            if (d !== -1)
                sketch.scaleFactor = d / c.valA
        }
    }
}
