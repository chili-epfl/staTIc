import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity{

    property quaternion char_mesh_rotation:
        quaternion_helper.product(
            structureLoaderTransform.fromEulerAngles(structureLoaderTransform.euler_angles.x,0,structureLoaderTransform.euler_angles.z),
                                  structureLoaderTransform.inv_rotation)

    property SphereMesh sphere_mesh:SphereMesh{
        radius: 2
    }
    property PhongAlphaMaterial phon_alpha_05:
        PhongAlphaMaterial{
        alpha: 0.5
    }
    property PhongMaterial phon_alpha_1:
        PhongMaterial{
    }
    property Mesh tiny_arrow_mesh:     Mesh{
        source:"qrc:/element_views/Element_Views/tiny_arrow.obj"
    }

    property PhongMaterial label_material:
        PhongMaterial{
        ambient:"#2C3539"
    }

    property PhongMaterial phong_material_red:
        PhongMaterial{
        ambient:"red"
    }

    property PhongMaterial phong_material_yellow:
        PhongMaterial{
        ambient:"yellow"
    }

    property PhongMaterial phong_material_green:
        PhongMaterial{
        ambient:"green"
    }
    property Mesh pinned_support_mesh:
        Mesh{
        source: "qrc:/UIMesh/3DObjects/pin support.obj"

    }
    property Mesh rolling_support_mesh:
        Mesh{
        source: "qrc:/UIMesh/3DObjects/rolling support.obj"
    }
    property Mesh fixed_support_mesh:
        Mesh{
        source: "qrc:/UIMesh/3DObjects/fix support.obj"
    }
    property Mesh empty_mesh:
        Mesh{
    }
}
