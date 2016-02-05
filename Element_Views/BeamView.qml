import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

import QtQuick 2.0 as QQ2
import QtPhysics.unofficial 1.0

Entity{
    id:rootEntity

    readonly property string type: "beam"

    property bool visible:  applicationRoot.currentViewFilter=='BEAM'
                            && backgroundsubtraction.entropy < .10
                            && (infobox.current_item == null || infobox.current_item == rootEntity) ? true : false

    property vector3d extreme1
    property vector3d extreme2

    property var segments

    property real length: extreme1.minus(extreme2).length()
    property size beamSize

    property int axialForceType: 0 //-1 compression,0 nul, 1 tension
    property real axialForce : 0

    //property real scaleFactor:2*(Math.abs(axialForce)-minForce)/(maxForce-minForce) + 1
    property real scaleFactor:3

    property real relativeAxialStress: 0

//    onAxialForceTypeChanged: {
//        animation.stop();
//        var prevAnimationValue=animationValue
//        if(axialForceType>0){
//            animation.from=prevAnimationValue
//            animation.to=length/2+prevAnimationValue
//        }
//        else{
//            animation.to=prevAnimationValue
//            animation.from=length/2+prevAnimationValue
//        }
//        animation.restart()
//    }


    property int nModels: 20
    property matrix4x4 poseMatrix

    property int animationValue: 0
    property int step: 10
    property int module: Math.round(length/2 - step)

    onExtreme1Changed: computeTransform()
    onExtreme2Changed: computeTransform()


    function computeTransform(){
        var a=Qt.vector3d(0,1,0);
        var b=extreme1.minus(extreme2).normalized();
        var axb=a.crossProduct(b);
        var result=Qt.matrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
        if(axb.x!==0 || axb.y!==0 || axb.z!==0){
            var tmp_ssc=ssc(axb);
            result=result.plus(tmp_ssc).plus(tmp_ssc.times(tmp_ssc).times((1-a.dotProduct(b))/Math.pow(axb.length(),2)));
            result.m44=1;
        }else{
            if(b.y===-1){
                result.m11=-1;
                result.m22=-1;
            }
        }
        var center=extreme1.plus(extreme2).times(0.5);
        result.m14=center.x;
        result.m24=center.y;
        result.m34=center.z;
        poseMatrix=result;
    }

    function ssc(v){
        var matrix=Qt.matrix4x4(0, -v.z, v.y, 0,
                                v.z, 0, -v.x, 0,
                                -v.y, v.x, 0, 0,
                                0   ,   0, 0, 1)
        return matrix;
    }


    CylinderMesh{
        id:mesh
        radius: 0.5
        length: parent.length
        enabled:  false
    }

    Transform{
        id:transform
        matrix: poseMatrix
    }

    components: [mesh,transform]

    Entity{
        enabled: visible
        SphereMesh{
            id:overview_mesh
            radius: 10
        }
        PhongAlphaMaterial{
            id:overview_material
            property real s: relativeAxialStress > 0.0001 ? Math.min(relativeAxialStress+0.15,1) : relativeAxialStress
            property real h: axialForceType>0 ? (0)/360 : (240)/360
            ambient:Qt.hsla(h,s,0.5)
            diffuse:"grey"
            specular:"black"
            shininess:0
            alpha:0.85
            QQ2.Behavior on ambient{
                QQ2.ColorAnimation{
                    duration: 500
                }
            }
        }
        Transform{
            id:overview_scale
            property real delta: 1*Math.min(relativeAxialStress,1)
            scale3D:Qt.vector3d(1-axialForceType*(delta),1+axialForceType*(delta),1-axialForceType*(delta));
            QQ2.Behavior on scale3D{
                QQ2.Vector3dAnimation{
                    duration: 1000
                }
            }
        }
        components: [overview_material,overview_mesh,overview_scale]
//        Entity{
//            Transform{
//                id:overview_shift_pos
//                translation:Qt.vector3d(0,10,0);

//            }
//            components: [overview_shift_pos,overview_mesh,overview_material]
//        }
//        Entity{
//            Transform{
//                id:overview_shift_neg
//                translation:Qt.vector3d(0,-10,0);

//            }
//            components: [overview_shift_neg,overview_mesh,overview_material]
//        }
    }



    /*-----Physical body----*/
    Entity{
       id:physicBody
       objectName: rootEntity.objectName
       CylinderMesh{
           enabled: true
           id:pBodyMesh
           radius: 10
           length: rootEntity.length
       }
       PhysicsBodyInfo{
           id:pBody
           kinematic:true
       }
       PhongAlphaMaterial{
           id:transparentMaterial
           alpha:0.2
       }

       components: [pBodyMesh,pBody,transparentMaterial]
    }
    /*-----Object Picker----*/
    Entity{
       SphereMesh{
           enabled: false
           id:objectPickerMesh
           radius: 10
       }

       property ObjectPicker objectPicker: ObjectPicker {
                   hoverEnabled: false
                   onClicked: {
                       if(rootEntity.visible)
                        infobox.current_item=rootEntity
                   }
       }
       components: [objectPickerMesh,objectPicker]
    }
    /*-----Reference bodies----*/
    Entity{
        components: [
            SphereMesh{
                id:extreme1Ref
                radius: 5
                enabled: infobox.current_item == rootEntity ? true: false
            },
            PhongMaterial{
                ambient:"#980000"
                diffuse:"black"
                specular:"black"
                shininess:0
            },
            Transform{
                translation:Qt.vector3d(0,length/2,0)
            } ]
    }
    Entity{
        components: [
            SphereMesh{
                id:extreme2Ref
                radius: 5
                enabled: infobox.current_item == rootEntity ? true: false
            },
            PhongMaterial{
                ambient:"#479800"
                diffuse:"black"
                specular:"black"
                shininess:0
            },
            Transform{
                translation:Qt.vector3d(0,-length/2,0)
            } ]
    }
    Entity{
        components: [
            SphereMesh{
                id:extreme3Ref
                radius: 5
                enabled: infobox.current_item == rootEntity ? true: false
            },
            PhongMaterial{
                ambient:"#001a98"
                diffuse:"black"
                specular:"black"
            },
            Transform{
                translation:Qt.vector3d(20,0,0)
            } ]
    }

//    QQ2.NumberAnimation on animationValue{
//        id:animation
//        duration: 10000
//        from: 0
//        to:  length/2
//        loops: QQ2.Animation.Infinite
//        running: visible && axialForceType!==0
//    }

    //First sequence

//    Mesh{
//        id:tiny_arrow
//        enabled: visible
//        source:"qrc:/element_views/Element_Views/tiny_arrow.obj"
//    }


//    AnimationUnitDy{
//        unitId: 0
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: 1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType < 0
//    }
//    AnimationUnitDy{
//        unitId: 1
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: 1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType < 0
//    }
//    AnimationUnitDy{
//        unitId: 2
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: 1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType < 0
//    }
//    AnimationUnitDy{
//        unitId: 3
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: 1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType < 0
//    }
//    AnimationUnitDy{
//        unitId: 4
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: 1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType < 0
//    }

//    AnimationUnitDy{
//        unitId: 0
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: -1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType > 0
//    }
//    AnimationUnitDy{
//        unitId: 1
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: -1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType > 0
//    }
//    AnimationUnitDy{
//        unitId: 2
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: -1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType > 0
//    }
//    AnimationUnitDy{
//        unitId: 3
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: -1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType > 0
//    }
//    AnimationUnitDy{
//        unitId: 4
//        unitMesh: tiny_arrow
//        step: parent.step
//        animationValue: parent.animationValue
//        module: parent.module
//        direction: -1
//        scaleFactor: parent.scaleFactor
//        rotate: parent.axialForceType > 0
//    }

}
