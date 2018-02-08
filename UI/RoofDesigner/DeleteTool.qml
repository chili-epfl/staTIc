import QtQuick 2.0

Item{


    function onPressed(target,mouse){

    }

    function onReleased(target,mouse){

    }

    function abort(){}
    function onClicked(target,mouse){
        if(target.class_type=="Point" || target.class_type=="Line"){
                target.kill();
                sketch.constraints.apply(sketch)
                sketch.store_state(sketch.undo_buffer.length+1);
        }
    }



}
