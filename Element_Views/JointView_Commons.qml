import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.5 as QQ2
Entity{

    property quaternion char_mesh_rotation:
        quaternion_helper.product(
            structureLoaderTransform.fromEulerAngles(structureLoaderTransform.euler_angles.x,0,structureLoaderTransform.euler_angles.z),
                                  structureLoaderTransform.inv_rotation)

    DepthTest{
        id:depthTest
        depthFunction: DepthTest.Always
    }
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
        source:"qrc:/UIMesh/3DObjects/tiny_arrow.obj"
    }

    property PhongMaterial label_material:
        PhongMaterial{
        id:label_material
        ambient:"#2C3539"
        QQ2.Component.onCompleted: {
            for(var i=0;i<label_material.effect.techniques.length;i++){
                var effect=label_material.effect.techniques[i]
                effect.renderPasses[0].renderStates=depthTest
            }
        }
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
        id:phong_material_green
        QQ2.Component.onCompleted: {
            for(var i=0;i<phong_material_green.effect.techniques.length;i++){
                var effect=phong_material_green.effect.techniques[i]
                effect.renderPasses[0].renderStates=depthTest
            }
        }
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
    property PhongAlphaMaterial focus_view_color_centre: PhongAlphaMaterial{
        property real h: Math.max(1-settings.focus_view_equilibrium_distance,0.5)
        ambient:Qt.hsla(0.33,h,0.32)
        diffuse:"grey"
        specular:"#000000"
        shininess:0
        alpha: h
        QQ2.Behavior on h {
            QQ2.NumberAnimation{duration: 1000}
        }

    }

    property PhongMaterial focus_view_color_target:
        PhongMaterial{
        id:focus_view_color_target
        property real h:0.33-0.23*settings.focus_view_equilibrium_distance
        ambient:Qt.hsla(h,1,0.32)
        diffuse:"grey"
        specular:"#000000"
        shininess:0
        QQ2.Behavior on h {
            QQ2.NumberAnimation{duration: 1000}
        }
        QQ2.Component.onCompleted: {
            for(var i=0;i<focus_view_color_target.effect.techniques.length;i++){
                var effect=focus_view_color_target.effect.techniques[i]
                effect.renderPasses[0].renderStates=depthTest
            }
        }

    }

    property Transform focus_view_target_transform:
    Transform{
        translation: settings.focus_view_currentForce.length()>0.001 ?
                         settings.focus_view_currentForce.times(settings.focus_view_scaleFactor_focus):
                         Qt.vector3d(0,0,0)
        QQ2.Behavior on translation{
            QQ2.Vector3dAnimation{
                duration: 500
            }
        }
    }
    property SphereMesh sphere_mesh_r3:SphereMesh{
        radius: 3
    }

}
