import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "nodeForce"
    property var backend_entity
    property int warehouse_index: 0
    property vector3d globalForce: Qt.vector3d(0,-weight*9.8,0);

    QQ2.Binding{
        when: backend_entity
        target: backend_entity
        property: "force"
        value:globalForce
    }

    property url asset3DMeshURL:warehouse3d.get(warehouse_index,"properties")["main_asset_url"];
    property url asset3DTextureURL:warehouse3d.get(warehouse_index,"properties")["main_asset_diffuse_map_url"];
    property real weight:warehouse3d.get(warehouse_index,"properties")["weight"];

    signal killMe();
    onKillMe: backend_entity.killMe()


    property vector3d offsetAugmentation: rootEntity.parent? Qt.vector3d(0,0,
                                                                         0)://-rootEntity.parent.tangibleSection.width/2):
                                                             Qt.vector3d(0,0,0)


    /*Visual aspect*/
    Transform{
        id:root_transform
        //rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
        translation:offsetAugmentation
    }


    property Mesh customMesh: Mesh{
        source: asset3DMeshURL
    }

    property DiffuseMapMaterial material_diffuse: DiffuseMapMaterial{
        ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        shininess: 2.0
        diffuse: asset3DTextureURL
    }

    property Material material: isSelected  ? trapz_force_commons.material_for_selection :material_diffuse

    property bool isSelected: infobox.current_item===rootEntity

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

}
