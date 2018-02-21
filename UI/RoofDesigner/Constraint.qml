import QtQuick 2.0

Item {
    readonly property string class_type: "Constraint"

    property bool existing: private_existing ? ( type===0 || type===1 ? entityA!==null && entityA.existing :
                                                                        type===2 ? ptA!==null && ptB!==null && ptA!==ptB && ptA.existing && ptB.existing :
                                                                                   type===3 ? entityA!==null && entityB!==null && entityA!==entityB && entityA.existing && entityB.existing :
                                                                                              type===4 ? ptA!==null && ptB!==null && ptA!==ptB && ptA.existing && ptB.existing :
                                                                                                         type===5 ? entityA!==null && entityB!==null && entityA!==entityB &&entityA.existing && entityB.existing :
                                                                                                                    type===6 ? entityA!==null && entityB!==null && entityA!==entityB &&entityA.existing && entityB.existing :
                                                                                                                               type===7 ? entityA!==null && entityB!==null && entityA!==entityB &&entityA.existing && entityB.existing :
                                                                                                                                          type===8 ? entityA!==null && ptA!==null && ptA.existing && entityA.existing :
                                                                                                                                                     type===9 ? ptA!==null && ptA.existing : false) :false

    property bool private_existing: true

    property string type_string: type==0 ? "Horizontal" :
                                           type==1 ? "Vertical" :
                                                     type==2 ? "Distance" :
                                                               type==3 ? "Equal Length" :
                                                                         type==4 ? "Distance" :
                                                                                   type==5 ? "Parallel":
                                                                                             type==6 ?"Perpendicular":
                                                                                                       type==7 ? "Angle" :
                                                                                                                 type==8 ? "MidPoint" :
                                                                                                                           type==9 ? "Where Dragged" : "Invalid"
    property int type: -1
    property double valA: -1.0
    property var ptA: null
    property var ptB: null
    property var entityA: null
    property var entityB: null

    property bool conflicting: false

    property var undo_buffer:[]
    property var redo_buffer:[]

    onParentChanged: {
        if(parent.class_type && parent.class_type=="Sketch"){
            objectName= parent.index_constraints+1
            parent.index_constraints=parent.index_constraints+1
        }
    }

    Connections{
        ignoreUnknownSignals: true
        target: parent
        onStore_state: store_state(epoch)
        onUndo: undo()
        onRedo: redo()
    }

    Connections{
        ignoreUnknownSignals: false
        target: (type==4 || type==2 || type==8) &&  existing && ptA!==undefined ? ptA : null
        onReplaceMe:{
            ptA=replacement
        }
    }

    Connections{
        ignoreUnknownSignals: false
        target: (type==4 || type==2) &&  existing && ptB!==undefined  ? ptB : null
        onReplaceMe:{
            ptB=replacement
        }
    }

    function undo(){
        if(undo_buffer.length>0){
            redo_buffer.push(undo_buffer.pop())
            if(undo_buffer.length>0){
                var state=undo_buffer[undo_buffer.length-1]
                type=state.type
                valA=state.valA
                ptA=state.ptA
                ptB=state.ptB
                entityA=state.entityA
                entityB=state.entityB
                private_existing=state.private_existing
            }else
                private_existing=false
        }
    }

    function redo(){
        if(redo_buffer.length>0){
            var state=redo_buffer.pop()
            type=state.type
            valA=state.valA
            ptA=state.ptA
            ptB=state.ptB
            entityA=state.entityA
            entityB=state.entityB
            private_existing=state.private_existing
            undo_buffer.push(state)
        }
    }

    function store_state(epoch){
        if(undo_buffer.length!=epoch-1){
            for(var i=0;i<epoch-1;i++)
                undo_buffer.push({
                                     'type':-1,
                                     'valA':-1.0,
                                     'ptA':null,
                                     'ptB':null,
                                     'entityA':null,
                                     'entityB':null,
                                     'private_existing':false})
        }
        var state={
            'type':type,
            'valA':valA,
            'ptA':ptA,
            'ptB':ptB,
            'entityA':entityA,
            'entityB':entityB,
            'private_existing':private_existing}
        undo_buffer.push(state)
        redo_buffer=[]
    }

    function kill(){
        private_existing=false

    }
}
