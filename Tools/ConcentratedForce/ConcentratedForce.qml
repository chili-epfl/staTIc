import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtPhysics.unofficial 1.0
import QtQuick 2.3 as QQ2
import ARToolkit 1.0

Entity {
    id:root
    property ARToolkitObject tag;
    enabled: settings.enable_tangible_load
    property var backend_entity

    property var last_target
    property real collision_local
    property vector3d last_collision_position
    property bool need_update: last_collision_position.minus(tag.translation).length()>50

    property bool attached: false
    onNeed_updateChanged: if(need_update) attached=false

    QQ2.Binding{
        when: backend_entity
        target: backend_entity
        property: "controlled_y"
        value:collision_local
    }


    function reset(){
        attached=false
        backend_entity=undefined
        last_target=undefined
    }
    Entity{
        SphereMesh{
            enabled: true
            id:extreme1Mesh
            radius: 10
        }
        Transform{
            id:extreme1Transform
            translation:tag.translation
        }
        PhysicsBodyInfo{
            id:extreme1Body
            kinematic:true
            inputTransform: extreme1Transform;
            mask: 1;
            group: 2;
            onCollisionsListChanged: {
                if(!attached){
                    if(collisionsList.length==0){
                        backend_entity.killMe();
                        reset();
                    }
                    else{
                        var is_new_target=true
                        var target=structureEntity.findEntityByPhysicsID(collisionsList[0].target)
                        var collision=collisionsList[0]
                        if(last_target)
                            for(var i=0;i<collisionsList.length;i++){
                                if(collisionsList[i].target==last_target){
                                    target=last_target
                                    is_new_target=false
                                    collision=collisionsList[i]
                                    break
                                }
                            }
                        if(target){
                            if(is_new_target){
                                if(backend_entity){
                                    backend_entity.killMe();
                                }
                                staticsModule.createTPZLoadTangible(target.backend_entity,target, root)
                                backend_entity.tangible_controlled=true
                            }
                            attached=true;
                            collision_local=collision.contactPointOnTargetLocal.y;
                            last_collision_position=tag.translation
                            last_target=target
                            backend_entity
                        }
                    }
                }
            }
        }
        components:[extreme1Body,extreme1Mesh,extreme1Transform]
    }

}

