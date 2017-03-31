import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2

Entity{

    property alias material_reaction: material_reaction
    property alias material_240_360: material_240_360
    property alias material_0_360: material_0_360

    DepthTest{
        id:depthTest
        depthFunction: DepthTest.LessOrEqual

    }

    PhongMaterial{
        id:material_reaction
        diffuse:Qt.hsla(120/360,1,0.5)
        shininess:0
        QQ2.Component.onCompleted: {
                for(var i=0;i<material_reaction.effect.techniques.length;i++){
                    var effect=material_reaction.effect.techniques[i]
                    effect.renderPasses[0].renderStates= depthTest
                }

        }
    }
    PhongMaterial{
        id:material_240_360
        diffuse:Qt.hsla(240/360,1,0.5)
        shininess:0
        QQ2.Component.onCompleted: {
                for(var i=0;i<material_240_360.effect.techniques.length;i++){
                    var effect=material_240_360.effect.techniques[i]
                    effect.renderPasses[0].renderStates= depthTest
                }

        }
    }
    PhongMaterial{
        id:material_0_360
        diffuse:Qt.hsla(0,1,0.5)
        shininess:0
        QQ2.Component.onCompleted: {
                for(var i=0;i<material_0_360.effect.techniques.length;i++){
                    var effect=material_0_360.effect.techniques[i]
                    effect.renderPasses[0].renderStates= depthTest
                }

        }
    }


}
