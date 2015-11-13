import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.0 as QQ2

Entity{

    id:root
    property bool visible:  applicationRoot.currentViewFilter=='JOINT' ? true : false

    property vector3d extreme1
    property vector3d extreme2

    property int axialForceType: 0 //-1 compression,0 nul, 1 tension
    property real axialForce : 0

    onAxialForceChanged:{
        if(axialForce<10)
            forceRadius=forceRadii["small"]
        else if (axialForce<20){
            forceRadius=forceRadii["medium"]
        }
        else{
            forceRadius=forceRadii["big"]
        }
    }

    property int nModels: 5
    property matrix4x4 poseMatrix

    property int animationValue: 0
    property int step: 10
    property int forceRadius: forceRadii["small"]

    property var forceRadii: { "small": 1, "medium": 2, "big":3 }

    QQ2.NumberAnimation on animationValue{
            duration: 10000
            from: axialForceType<0 ? 50 : 0
            to: axialForceType<0 ? 0 : 50
            loops: QQ2.Animation.Infinite
            running: visible && axialForceType!==0
    }

    onExtreme1Changed: computeTransform()
    onExtreme2Changed: computeTransform()


    function computeTransform(){
        var a=Qt.vector3d(1,0,0);
        var b=extreme2.minus(extreme1).normalized();
        var axb=a.crossProduct(b);
        var result=Qt.matrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
        if(axb.x!==0 || axb.y!==0 || axb.z!==0){
            var tmp_ssc=ssc(axb);
            result=result.plus(tmp_ssc).plus(tmp_ssc.times(tmp_ssc).times((1-a.dotProduct(b))/Math.pow(axb.length(),2)));
            result.m44=1;
        }else{
            if(b.x===-1){
                result.m11=-1;
                result.m22=-1;
            }
        }
//        var center=extreme1.plus(extreme2).times(0.5);
//        result.m14=center.x;
//        result.m24=center.y;
//        result.m34=center.z;

        poseMatrix=result;
    }
    function ssc(v){
        var matrix=Qt.matrix4x4(0, -v.z, v.y, 0,
                                v.z, 0, -v.x, 0,
                                -v.y, v.x, 0, 0,
                                0   ,   0, 0, 1)
        return matrix;
    }



    Mesh{
        id:tiny_arrow
        enabled: visible
        source:"qrc:/element_views/Element_Views/tiny_arrow.obj"
    }

    Entity{
        AnimationUnitDx{
            unitId: 0
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            rotate: root.axialForceType < 0
        }

        AnimationUnitDx{
            unitId: 1
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 2
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 3
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 4
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            rotate: root.axialForceType < 0
        }



        components: [  Transform{
                MatrixTransform{
                    matrix: poseMatrix
                }
            }]
    }

//    Entity{
//        components: [
//        SphereMesh{
//            enabled: visible
//            radius: forceRadius;
//            },
//        Transform{
//            Translate{
//                dx:(0*step+animationValue)%50
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }

//    Entity{
//        components: [
//        SphereMesh{
//            enabled: visible
//            radius: forceRadius;
//            },
//        Transform{
//            Translate{
//                dx:(1*step+animationValue)%50
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }
//    Entity{
//        components: [
//        SphereMesh{
//            enabled: visible
//            radius: forceRadius;
//            },
//        Transform{
//            Translate{
//                dx:(2*step+animationValue)%50
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }
//    Entity{
//        components: [
//        SphereMesh{
//            enabled: visible
//            radius: forceRadius;
//            },
//        Transform{
//            Translate{
//                dx:(3*step+animationValue)%50
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }
//    Entity{
//        components: [
//        SphereMesh{
//            enabled: visible
//            radius: forceRadius;
//            },
//        Transform{
//            Translate{
//                dx:(4*step+animationValue)%50
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }


//    NodeInstantiator{
//        model:nModels
//        //asynchronous:true
//        delegate:
//            Entity{
//            id:animationUnit
//            property int idx: index
//            SphereMesh{
//                enabled: visible
//                id:auMesh
//                radius: forceRadius;            }
//            Transform{
//                id:auTransform
//                Translate{
//                    dx:(idx*step+animationValue)%50
//                }
//                MatrixTransform{
//                    matrix: poseMatrix
//                }
//            }
//            components: [auMesh,auTransform]
//        }
//    }

}
