import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity{

    property CylinderMesh main_mesh_cylinder:
        CylinderMesh{
        radius: 2.5
        length: 5
    }

//    property PhongAlphaMaterial phong_alpha_75_material_green: PhongAlphaMaterial{
//        ambient:  "green"
//        diffuse:"grey"
//        specular:"black"
//        alpha:0.75
//    }

//    property PhongAlphaMaterial phong_alpha_75_material_grey: PhongAlphaMaterial{
//        ambient:  "grey"
//        diffuse:"grey"
//        specular:"black"
//        alpha:0.75
//    }

    property SphereMesh sphere_mesh:
        SphereMesh{
        radius: 7.5
    }

    property Mesh spring_mesh:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/spring.obj"
    }

    property PhongMaterial phong_grey:
        PhongMaterial{
        ambient: "#333333"
    }
    property SphereMesh sphere_mesh_reference:
        SphereMesh{
        radius: 5
    }
    property PhongMaterial phong_blue_reference:
        PhongMaterial{
        ambient:"#001a98"
        diffuse:"black"
        specular:"black"
    }
    property PhongAlphaMaterial transparent_material:
        PhongAlphaMaterial{
        alpha:0
    }

    property CylinderMesh drag_mesh:
        CylinderMesh{
        radius: 5
        length: 40
    }
    property PhongAlphaMaterial phong_alpha_70_material_green: PhongAlphaMaterial{
        ambient:  "green"
        alpha:0.75
    }
    property CylinderMesh dispalcement_mesh:
        CylinderMesh{
        radius: 2.5
        length: 5
    }
}
