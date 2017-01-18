import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    property url asset3DMeshURL:"qrc:/tools/Tools/TangibleTrapzLoad/tonweight.obj";
    property url asset3DTextureURL:"qrc:/tools/Tools/TangibleTrapzLoad/texture.png";
    property vector3d relativePosition: Qt.vector3d(0,0,0)
//    property vector3d offsetAugmentation: parent ? Qt.vector3d(parent.tangibleSection.height/2,0,
//                                                      -parent.tangibleSection.width/2) : Qt.vector3d(0,0,0)
    property vector3d offsetAugmentation: Qt.vector3d(0,0,0)
    property bool selected : false;
    Transform{
          id:transform
          translation:offsetAugmentation.plus(relativePosition);
          rotation:fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
    }
    Mesh{
        id:assetMesh
        source:asset3DMeshURL
    }
    DiffuseMapMaterial {
        id:material_diffuse
        ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        diffuse:  asset3DTextureURL
        shininess: 2.0
    }
    PhongAlphaMaterial{
        id:material_selection
        ambient:  "yellow"
        diffuse:"grey"
        specular:"black"
        alpha:0.80
    }
    property Material material: selected ? material_selection:material_diffuse


    components: [assetMesh,material,transform]

}
