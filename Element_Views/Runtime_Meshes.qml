import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    property var loaded_mesh:({})
    property var loaded_texture:({})

    property int first_available_mesh: 0
    property int first_available_texture: 0

    readonly property int max_num_resources: 25

    function getMesh(url){
        if(loaded_mesh.hasOwnProperty(url))
            return node_instatiator_mesh.objectAt(loaded_mesh[url]);
        else{
            if(first_available_mesh<max_num_resources){
                node_instatiator_mesh.objectAt(first_available_mesh).source=url;
                loaded_mesh[url]=first_available_mesh;
                first_available_mesh++;
                console.log(first_available_mesh)
                return node_instatiator_mesh.objectAt(loaded_mesh[url])
            }
        }
    }
    function getTexture(url){
        if(loaded_texture.hasOwnProperty(url))
            return node_instatiator_texture.objectAt(loaded_texture[url]);
        else{
            if(first_available_texture<max_num_resources){
                node_instatiator_texture.objectAt(first_available_texture).diffuse=url;
                loaded_texture[url]=first_available_texture;
                first_available_texture++;
                return node_instatiator_texture.objectAt(loaded_texture[url])
            }
        }
    }

   NodeInstantiator {
       id:node_instatiator_mesh
        model: max_num_resources;
        delegate:Mesh{

        }
    }

   NodeInstantiator {
       id:node_instatiator_texture
        model: max_num_resources;
        delegate:DiffuseMapMaterial{
            ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
            shininess: 2.0
        }
    }

}
