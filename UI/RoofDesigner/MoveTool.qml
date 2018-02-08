import QtQuick 2.0

Item{
    property var current_target:undefined
    property var line_component
    property var constraint_component
    property var last_move_constraint

    Component.onCompleted:  {
        line_component = Qt.createComponent("Line.qml");
        constraint_component = Qt.createComponent("Constraint.qml");
        last_move_constraint = constraint_component.createObject(sketch)
    }

    function onPressed(target,mouse){
        if(target.class_type=="Point"){
            target.mouse_area.drag.target=target
            current_target=target
            last_move_constraint.type = 9
            last_move_constraint.ptA = target
        }
    }

    function abort(){
        if(current_target!==undefined){
            current_target.mouse_area.drag.target=undefined
        }
        current_target=undefined
    }

    function onReleased(target,mouse){
        var lines_not_belonging=[]
        var lines_belonging=[]
        var constraints=[]
        if(target.class_type=="Point"){
            var p=target.mouse_area.drag.target
            for(var i=0;i<sketch.children.length;i++){
                if(p!==sketch.children[i] && sketch.children[i].class_type=="Point"
                        && sketch.children[i].existing
                        && Math.abs((p.x-sketch.children[i].x))<10 &&
                        Math.abs((p.y-sketch.children[i].y))<10){
                    p.replaceMe(sketch.children[i])
                    lines_not_belonging=[]
                    lines_belonging=[]
                    constraints=[]
                    break;
                }else if(sketch.children[i].class_type=="Line" &&
                         sketch.children[i].existing
                         ){
                    if(p!==sketch.children[i].p1 &&
                            p!==sketch.children[i].p2)
                        lines_not_belonging.push(sketch.children[i])
                    else
                        lines_belonging.push(sketch.children[i])

                }else if(sketch.children[i].class_type=="Constraint" &&
                         sketch.children[i].existing){
                    constraints.push(sketch.children[i])
                }
            }
            for(i=0;i<lines_not_belonging.length;i++){
                var line=lines_not_belonging[i]
                var intersection=checkIntersection(Qt.vector2d(p.x,p.y),
                                                   Qt.vector2d(line.p1.x,line.p1.y),
                                                   Qt.vector2d(line.p2.x,line.p2.y)
                                                   )
                if(intersection!==false){
                    var line_s1=line_component.createObject(sketch)
                    line_s1.p1=line.p1;
                    line_s1.p2=p
                    var line_s2=line_component.createObject(sketch)
                    line_s2.p2=line.p2;
                    line_s2.p1=p
                    //Propagate constraints: the "equal length" is no longer valid
                    var need_collinearity=true
                    for(var c=0;c<constraints.length;c++){
                        var constraint=constraints[c]
                        if(constraint.entityA===line || constraint.entityB===line){
                            if(constraint.type==0 ||
                                    constraint.type==1 ||
                                    constraint.type==5 ||
                                    constraint.type==6 ||
                                    constraint.type==7){
                                var constr_s1=constraint_component.createObject(sketch)
                                var constr_s2=constraint_component.createObject(sketch)
                                constr_s1.type=constraint.type;
                                constr_s2.type=constraint.type;
                                constr_s1.entityA=line_s1;
                                constr_s2.entityA=line_s2;
                                constr_s1.entityB=constraint.entityB!==line ? constraint.entityB :constraint.entityA
                                constr_s2.entityB=constraint.entityB!==line ? constraint.entityB :constraint.entityA
                                constr_s1.valA=constraint.valA
                                constr_s2.valA=constraint.valA
                                need_collinearity=false
                            }
                        }
                    }
                    line.kill();
                    if(need_collinearity){
                        var constr_collinear=constraint_component.createObject(sketch)
                        constr_collinear.type=5
                        constr_collinear.entityA=line_s1
                        constr_collinear.entityB=line_s2
                    }
                    for(var j=0;j<lines_belonging.length;j++){
                        //Remove Duplicates
                        //For these lines they vanish and  we just break the constraints
                        if((line_s1.p1===lines_belonging[j].p1 && line_s1.p2===lines_belonging[j].p2)
                                || (line_s1.p1===lines_belonging[j].p2 && line_s1.p2===lines_belonging[j].p1) )
                            lines_belonging[j].kill();
                        else  if((line_s2.p1===lines_belonging[j].p1 && line_s2.p2===lines_belonging[j].p2)
                                 || (line_s2.p1===lines_belonging[j].p2 && line_s2.p2===lines_belonging[j].p1) )
                            lines_belonging[j].kill();
                    }
                    break;
                }
            }
            target.mouse_area.drag.target=undefined;
            sketch.store_state(sketch.undo_buffer.length+1);
        }
        current_target=undefined
    }

    function onClicked(target,mouse){

    }

    Connections{
        Binding on target{
            when:current_target!==undefined
            value: current_target
        }
        ignoreUnknownSignals: true
        onXChanged:apply_constraint_timer.start()
        onYChanged:apply_constraint_timer.start()
    }
    Timer{
        id:apply_constraint_timer
        interval: 100
        running: false
        onTriggered: sketch.constraints.apply(sketch);
    }

    function checkIntersection(point, e1, e2){

        var dist=Math.abs( (e2.y-e1.y)*point.x -(e2.x-e1.x)*point.y +e2.x*e1.y-e2.y*e1.x)/
                (Math.sqrt ((e1.x-e2.x)*(e1.x-e2.x)+(e1.y-e2.y)*(e1.y-e2.y)))

        if(dist>10)
            return false;

        var v1 = point.minus(e1);
        var v2 = e2.minus(e1);

        var v3 = v2.normalized();

        var dot = v2.dotProduct(v3);

        var t1 = v2.times(v1).length()*(1/dot);
        var t2 = (v1.dotProduct(v3))*( 1/dot);

        if (t1 >= 0.0 && (t2 >= 0.0 && t2 <= 1.0))
            return true;

        v3 = v3.times(-1);

        dot = v2.dotProduct(v3);

        t1 = v2.times(v1).length()*(1/dot);
        t2 = (v1.dotProduct(v3))*( 1/dot);

        if (t1 >= 0.0 && (t2 >= 0.0 && t2 <= 1.0))
            return true;

        return false;

    }

}
