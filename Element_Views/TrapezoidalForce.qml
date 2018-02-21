import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "trapezoidalForce"
    property var backend_entity
    property int warehouse_index: 0
    property vector3d globalForce: Qt.vector3d(0,-weight*9.8,0);

    QQ2.Binding{
        when: backend_entity
        target: backend_entity
        property: "force"
        value:globalForce
    }
    QQ2.Binding{
        when: backend_entity
        target: backend_entity
        property: "relativePosition"
        value:relativeLocalPosition
    }
    QQ2.Binding{
        when: backend_entity
        target: backend_entity
        property: "extent"
        value:extent
    }

    property url asset3DMeshURL:warehouse3d.get(warehouse_index,"properties")["main_asset_url"];
    property url asset3DTextureURL:warehouse3d.get(warehouse_index,"properties")["main_asset_diffuse_map_url"];
    property real weight:warehouse3d.get(warehouse_index,"properties")["weight"];
    property vector2d extent:warehouse3d.get(warehouse_index,"properties")["extent"];

    signal killMe();
    onKillMe: backend_entity.killMe()

    onEnabledChanged: rootEntity.parent.disable_beam_selection_for_load=false


    property vector3d relativeLocalPosition:rootEntity.parent?
                                                Qt.vector3d(-root_transform.translation.y/rootEntity.parent.length + 0.5,-root_transform.translation.y/rootEntity.parent.length + 0.5,-root_transform.translation.y/rootEntity.parent.length + 0.5):
                                                Qt.vector3d(0,0,0);

    onRelativeLocalPositionChanged: {
        if(isSelected){
            var pos_text="("+relativeLocalPosition.x+","+relativeLocalPosition.y+","+relativeLocalPosition.z+")"
            logger.log("Trapezoidal_force_move", {"relative_position":pos_text,"beam":parent.objectName})        
        }
    }

    property vector3d offsetAugmentation: rootEntity.parent? Qt.vector3d(rootEntity.parent.tangibleSection.height/2,0,
                                                                         0)://-rootEntity.parent.tangibleSection.width/2):
                                                             Qt.vector3d(0,0,0)


    /*Visual aspect*/
    Transform{
        id:root_transform
        rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
        translation:offsetAugmentation
    }


    property Mesh customMesh: Mesh{
        source: asset3DMeshURL
    }
    property DiffuseMapMaterial material_diffuse: DiffuseMapMaterial{
        ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        shininess: 2.0
        diffuse: TextureLoader { source:asset3DTextureURL}
    }
    property Material material: isSelected  ? trapz_force_commons.material_for_selection :material_diffuse

    property bool isSelected: infobox.current_item===rootEntity

    onIsSelectedChanged: {
        if(isSelected)
            rootEntity.parent.disable_beam_selection_for_load=true
        else
            rootEntity.parent.disable_beam_selection_for_load=false
    }
    ObjectPicker {
        id:valid_picker
        hoverEnabled: false
        onClicked: {
            sceneRoot.mouseEventHasBeenAccepted=true;
            if(settings.load_is_selectable && !isSelected){
                infobox.current_item=rootEntity
            }
        }
    }

    Entity{
        components: rootEntity.enabled && !isSelected ?
                        [root_transform,customMesh,material,valid_picker]:
                        [root_transform,customMesh,material]
    }


    NodeInstantiator{
        id:picker_instantiator
        model: rootEntity.parent ? Math.floor((rootEntity.parent.length-20)/50)+1 :0
        property int central_index:model%2==0 ? 0.5*model+0.5 : Math.floor(0.5*model)
        delegate: Entity{
            enabled: settings.beam_is_selectable && isSelected
            property Transform transform: Transform{
                //rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                translation:rootEntity.parent?
                                Qt.vector3d(rootEntity.parent.tangibleSection.height/2+beam_commons.drag_mesh.radius,
                                        (index-picker_instantiator.central_index)*50,
                                        0):
                                Qt.vector3d(0,0,0)
            }
            property ObjectPicker objectPicker:ObjectPicker{
                onClicked: {
                    sceneRoot.mouseEventHasBeenAccepted=true;
                    root_transform.translation=rootEntity.offsetAugmentation.plus(Qt.vector3d(0,(index-picker_instantiator.central_index)*50,0));
                }
            }
            components:  rootEntity.enabled &&  isSelected ?
                             [beam_commons.drag_mesh,this.transform,this.objectPicker] :
                             []
            Entity{
                enabled: isSelected
                property Transform transform: Transform{
                    scale3D:Qt.vector3d(0.5,0.5,0.5)
                }
                components:  rootEntity.enabled ?
                                 [this.transform,beam_commons.drag_mesh,beam_commons.phong_material_green]:
                                 []
            }
        }
    }

}
