import QtQuick 2.0

Item{
    property var point_component
    property var line_component
    property var constraint_component

    Component.onCompleted:  {
        line_component = Qt.createComponent("Line.qml");
        point_component = Qt.createComponent("Point.qml");
        constraint_component = Qt.createComponent("Constraint.qml");
    }

    property var unfinished_p1: undefined
    property var unfinished_p2: undefined
    property var unfinished_line: undefined


    function abort(){
        if(unfinished_p1!==undefined)
            unfinished_p1.destroy()
        if(unfinished_p2!==undefined)
            unfinished_p2.destroy()
        if(unfinished_line!==undefined)
            unfinished_line.destroy()

        unfinished_p1= undefined
        unfinished_p2= undefined
        unfinished_line= undefined


    }

    function onPressed(target,mouse){
        var p1;
        if(target.class_type=="Sketch"){
            p1=point_component.createObject(sketch)
            p1.x=mouse.x-p1.width/2
            p1.y=mouse.y-p1.width/2
            unfinished_p1=p1
        }
        else if(target.class_type=="Point"){
            p1=target
        }else if(target.class_type=="Line"){
            p1=point_component.createObject(sketch)
            var global_pos=target.mapToItem(sketch,mouse.x,mouse.y)
            p1.x=global_pos.x-p1.width/2
            p1.y=global_pos.y-p1.width/2
            unfinished_p1=p1
        }
        var p2=point_component.createObject(sketch)
        var pos=target.mapToItem(sketch,mouse.x,mouse.y)
        p2.x=pos.x-p2.width/2
        p2.y=pos.y-p2.width/2
        target.mouse_area.drag.target=p2
        var line=line_component.createObject(sketch)
        line.p1=p1
        line.p2=p2
        unfinished_p2=p2
        unfinished_line=line
    }

    function onReleased(target,mouse){
        var lines_not_belonging=[]
        var lines_belonging=[]
        var constraints=[]

        var p2=target.mouse_area.drag.target
        for(var i=0;i<sketch.children.length;i++){
            if(p2!==sketch.children[i] && sketch.children[i].class_type=="Point"                     
                    && sketch.children[i].existing &&
                    Math.abs((p2.x-sketch.children[i].x))<10 &&
                       Math.abs((p2.y-sketch.children[i].y))<10){
                p2.replaceMe(sketch.children[i])
                lines_not_belonging=[]
                lines_belonging=[]
                constraints=[]
                break;
            }else if(sketch.children[i].class_type=="Line" &&
                     sketch.children[i].existing
                     ){
                if(p2!==sketch.children[i].p1 &&
                        p2!==sketch.children[i].p2)
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
            var intersection=checkIntersection(Qt.vector2d(p2.x,p2.y),
                                               Qt.vector2d(line.p1.x,line.p1.y),
                                               Qt.vector2d(line.p2.x,line.p2.y)
                                               )
            if(intersection!==false){
                var line_s1=line_component.createObject(sketch)
                line_s1.p1=line.p1;
                line_s1.p2=p2
                var line_s2=line_component.createObject(sketch)
                line_s2.p2=line.p2;
                line_s2.p1=p2
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

        //Intersection checks for p1
        lines_not_belonging=[]
        lines_belonging=[]
        constraints=[]
        if(unfinished_p1!==undefined){
            for(var i=0;i<sketch.children.length;i++){
                if(sketch.children[i].class_type=="Line" &&
                        sketch.children[i].existing
                        ){
                    if(unfinished_p1!==sketch.children[i].p1 &&
                            unfinished_p1!==sketch.children[i].p2)
                        lines_not_belonging.push(sketch.children[i])
                    else
                        lines_belonging.push(sketch.children[i])

                }else if(sketch.children[i].class_type=="Constraint" &&
                         sketch.children[i].existing){
                    constraints.push(sketch.children[i])
                }
            }
            for(i=0;i<lines_not_belonging.length;i++){
                line=lines_not_belonging[i]
                intersection=checkIntersection(Qt.vector2d(unfinished_p1.x,unfinished_p1.y),
                                               Qt.vector2d(line.p1.x,line.p1.y),
                                               Qt.vector2d(line.p2.x,line.p2.y)
                                               )
                if(intersection!==false){
                    line_s1=line_component.createObject(sketch)
                    line_s1.p1=line.p1;
                    line_s1.p2=unfinished_p1
                    line_s2=line_component.createObject(sketch)
                    line_s2.p2=line.p2;
                    line_s2.p1=unfinished_p1
                    //Propagate constraints: the "equal length" is no longer valid
                    need_collinearity=true
                    for(var c=0;c<constraints.length;c++){
                        constraint=constraints[c]
                        if(constraint.entityA===line || constraint.entityB===line){
                            if(constraint.type==0 ||
                                    constraint.type==1 ||
                                    constraint.type==5 ||
                                    constraint.type==6 ||
                                    constraint.type==7){
                                constr_s1=constraint_component.createObject(sketch)
                                constr_s2=constraint_component.createObject(sketch)
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
                        constr_collinear=constraint_component.createObject(sketch)
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

        }
        target.mouse_area.drag.target=undefined;
        sketch.store_state(sketch.undo_buffer.length+1);
        unfinished_p1= undefined
        unfinished_p2= undefined
        unfinished_line= undefined
    }

    function onClicked(target,mouse){


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
