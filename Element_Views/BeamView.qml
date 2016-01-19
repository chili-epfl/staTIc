import Qt3D 2.0
import Qt3D.Renderer 2.0
import Qt3D.Input 2.0

import QtQuick 2.0 as QQ2
import QtPhysics.unofficial 1.0
Entity{
    id:rootEntity
    objectName: "Beam"
    property bool visible:  applicationRoot.currentViewFilter=='BEAM' && backgroundsubtraction.entropy < .10 ? true : false

    property vector3d extreme1
    property vector3d extreme2

    property real length: extreme1.minus(extreme2).length()

    property int axialForceType: 0 //-1 compression,0 nul, 1 tension
    property real axialForce : 0

    //property real scaleFactor:2*(Math.abs(axialForce)-minForce)/(maxForce-minForce) + 1
    property real scaleFactor:3

    onAxialForceTypeChanged: {
        animation.stop();
        var prevAnimationValue=animationValue
        if(axialForceType>0){
            animation.from=prevAnimationValue
            animation.to=length/2+prevAnimationValue
        }
        else{
            animation.to=prevAnimationValue
            animation.from=length/2+prevAnimationValue
        }
        animation.restart()
    }


    property int nModels: 20
    property matrix4x4 poseMatrix

    property int animationValue: 0
    property int step: 10
    property int module: Math.round(length/2 - step)

    QQ2.NumberAnimation on animationValue{
        id:animation
        duration: 10000
        from: 0
        to:  length/2
        loops: QQ2.Animation.Infinite
        running: visible && axialForceType!==0
    }

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
        MatrixTransform{
            matrix: poseMatrix
        }
    }

    components: [mesh,transform]

    Entity{
       id:physicBody
       CylinderMesh{
           enabled: false
           id:pBodyMesh
           radius: 10
           length: rootEntity.length
       }
       PhysicsBodyInfo{
           id:pBody
           kinematic:true
       }
       MouseInput {
           id:mouseInput
           controller: mouseController
           onClicked: {
               //Signal not implemented yet
               console.log("click")

           }
           onReleased: {
               console.log("click")
               switch (mouse.button) {
               case Qt.LeftButton:
                   console.log("click")
                   break;
               }
           }
           QQ2.Component.onCompleted: {
                console.log(mouseInput.controller)
           }
       }

       components: [pBodyMesh,pBody,mouseInput]
    }


    //First sequence

    Mesh{
        id:tiny_arrow
        enabled: visible
        source:"qrc:/element_views/Element_Views/tiny_arrow.obj"
    }


    AnimationUnitDy{
        unitId: 0
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: 1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType < 0
    }
    AnimationUnitDy{
        unitId: 1
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: 1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType < 0
    }
    AnimationUnitDy{
        unitId: 2
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: 1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType < 0
    }
    AnimationUnitDy{
        unitId: 3
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: 1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType < 0
    }
    AnimationUnitDy{
        unitId: 4
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: 1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType < 0
    }

    AnimationUnitDy{
        unitId: 0
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: -1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType > 0
    }
    AnimationUnitDy{
        unitId: 1
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: -1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType > 0
    }
    AnimationUnitDy{
        unitId: 2
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: -1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType > 0
    }
    AnimationUnitDy{
        unitId: 3
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: -1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType > 0
    }
    AnimationUnitDy{
        unitId: 4
        unitMesh: tiny_arrow
        step: parent.step
        animationValue: parent.animationValue
        module: parent.module
        direction: -1
        scaleFactor: parent.scaleFactor
        rotate: parent.axialForceType > 0
    }

//    property int oscillator : 0
//    property real randomFactor: 1
//    QQ2.SequentialAnimation{
//           running: true
//           loops: QQ2.Animation.Infinite
//           QQ2.NumberAnimation{
//               target: rootEntity
//               property: "oscillator"
//               duration: 200
//               from: -1
//               to:  0
//           }
////           QQ2.NumberAnimation{
////               target: rootEntity
////               property: "oscillator"
////               duration: 200
////               from: 1
////               to:  -1
////           }
//    }


//    Entity{

//        MatrixTransform{
//            id:removeRotationTransform
//            matrix:Qt.matrix4x4(structureLoaderTransform.matrix.m11,structureLoaderTransform.matrix.m12,structureLoaderTransform.matrix.m13,0,
//                                structureLoaderTransform.matrix.m21,structureLoaderTransform.matrix.m22,structureLoaderTransform.matrix.m23,0,
//                                structureLoaderTransform.matrix.m31,structureLoaderTransform.matrix.m32,structureLoaderTransform.matrix.m33,0,
//                                structureLoaderTransform.matrix.m41,structureLoaderTransform.matrix.m42,structureLoaderTransform.matrix.m43,1).inverted()
//        }

//        components: [removeRotationTransform]

//        Entity{
//            components: [SphereMesh{
//                    enabled: true
//                    radius: 1;
//                }
//                , Transform{
//                    Translate{
//                        dx: oscillator==0 ?  Math.random()*randomFactor :  -Math.random()*randomFactor;
//                        dy:(1*step);
//                    }
//                }]
//        }

//        Entity{
//            components: [SphereMesh{
//                    enabled: true
//                    radius: 1;
//                }
//                , Transform{
//                    Translate{
//                        dx: oscillator==0 ? -Math.random()*randomFactor :  Math.random()*randomFactor;
//                        dy:(2*step);
//                    }
//                }]
//        }

//        Entity{
//            components: [SphereMesh{
//                    enabled: true
//                    radius: 1;
//                }
//                , Transform{
//                    Translate{
//                        dx: oscillator==0 ?  Math.random()*randomFactor :  -Math.random()*randomFactor;
//                        dy:(3*step);
//                    }
//                }]
//        }


//    }






}
