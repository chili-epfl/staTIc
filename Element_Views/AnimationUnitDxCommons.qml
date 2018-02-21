import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2
import PhongMaterialCustomDepthTest 1.0
Entity{

    property alias material_reaction: material_reaction
    property alias material_240_360: material_240_360
    property alias material_0_360: material_0_360

    DepthTest{
        id:depthTest
        depthFunction: DepthTest.LessOrEqual

    }

    PhongMaterialCustomDepthTest{
        depthTest: depthTest
        id:material_reaction
        diffuse:Qt.hsla(120/360,1,0.5)
        shininess:0
    }
    PhongMaterialCustomDepthTest{
        depthTest: depthTest
        id:material_240_360
        diffuse:Qt.hsla(240/360,1,0.5)
        shininess:0

    }
    PhongMaterialCustomDepthTest{
        depthTest: depthTest
        id:material_0_360
        diffuse:Qt.hsla(0,1,0.5)
        shininess:0

    }


}
