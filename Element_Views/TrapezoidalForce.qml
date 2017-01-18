import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
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


    property Mesh customMesh: runtime_meshes.getMesh(asset3DMeshURL)
    property DiffuseMapMaterial material_diffuse: runtime_meshes.getTexture(asset3DTextureURL)

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
        model: Math.floor((rootEntity.parent.length-20)/50)+1
        property int central_index:model%2==0 ? 0.5*model+0.5 : Math.floor(0.5*model)
        delegate: Entity{
            enabled: settings.beam_is_selectable && isSelected
            property Transform transform: Transform{
                //rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                translation:Qt.vector3d(rootEntity.parent.tangibleSection.height/2+beam_commons.drag_mesh.radius,
                                        (index-picker_instantiator.central_index)*50,
                                        0)
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
