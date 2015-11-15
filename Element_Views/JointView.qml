import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.3 as QQ2

Entity{
    id:root
    property bool visible: applicationRoot.currentViewFilter=='JOINT' ? true : false

    property vector3d position
    property vector3d reaction

    property real reactionMagnitude: reaction.length()

    property real scaleFactor: 2*(reactionMagnitude-minForce)/(maxForce-minForce) + 1


    onPositionChanged: computeTransform()
    onReactionChanged:{
        computeTransform();
    }

    property matrix4x4 poseMatrix

    property int animationValue: 0
    property int step: 10

    property int distanceFromJoint: 50

    QQ2.NumberAnimation on animationValue{
            id:animation
            duration: 10000
            from:  0
            to: distanceFromJoint
            loops: QQ2.Animation.Infinite
            running: visible && reactionMagnitude > 0
    }

    function computeTransform(){
        var a=Qt.vector3d(1,0,0);
        var b=reaction.normalized();
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
        result.m14=-b.x*distanceFromJoint
        result.m24=-b.y*distanceFromJoint
        result.m34=-b.z*distanceFromJoint
        poseMatrix=result;
    }
    function ssc(v){
        var matrix=Qt.matrix4x4(0, -v.z, v.y, 0,
                                v.z, 0, -v.x, 0,
                                -v.y, v.x, 0, 0,
                                0   ,   0, 0, 1)
        return matrix;
    }
    SphereMesh{
        id:mesh
        radius: 2
        enabled:  visible
    }

    Transform{
        id:transform
        Translate{
            dx:position.x
            dy:position.y
            dz:position.z
        }
    }
    components: [mesh,transform]



    Mesh{
        id:tiny_arrow
        enabled: visible && reactionMagnitude>0
        source:"qrc:/element_views/Element_Views/tiny_arrow.obj"
    }


    Entity{
        AnimationUnitDx{
            unitId: 0
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: distanceFromJoint
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: false
        }

        AnimationUnitDx{
            unitId: 1
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: distanceFromJoint
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: false
        }
        AnimationUnitDx{
            unitId: 2
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: distanceFromJoint
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: false
        }
        AnimationUnitDx{
            unitId: 3
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: distanceFromJoint
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: false
        }
        AnimationUnitDx{
            unitId: 4
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: distanceFromJoint
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: false
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
//            enabled: visible && reactionMagnitude>0
//            id:ruMesh
//            radius: forceRadius;
//        },
//        Transform{
//            Translate{
//                dx:((0)*step+animationValue)%distanceFromJoint
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
//            enabled: visible && reactionMagnitude>0
//            radius: forceRadius;
//        },
//        Transform{
//            Translate{
//                dx:((1)*step+animationValue)%distanceFromJoint
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
//            enabled: visible && reactionMagnitude>0
//            radius: forceRadius;
//        },
//        Transform{
//            Translate{
//                dx:((2)*step+animationValue)%distanceFromJoint
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
//            enabled: visible && reactionMagnitude>0
//            radius: forceRadius;
//        },
//        Transform{
//            Translate{
//                dx:((3)*step+animationValue)%distanceFromJoint
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
//            enabled: visible && reactionMagnitude>0
//            radius: forceRadius;
//        },
//        Transform{
//            Translate{
//                dx:((4)*step+animationValue)%distanceFromJoint
//            }
//            MatrixTransform{
//                matrix: poseMatrix
//            }
//        }
//        ]
//    }







//    NodeInstantiator{
//        model:5
//        //asynchronous:true
//        delegate:
//            Entity{
//            id:reactionUnit
//            property int idx: index
//            SphereMesh{
//                enabled: visible && reactionMagnitude>0
//                id:ruMesh
//                radius: forceRadius;
//            }
//            Transform{
//                id:ruTransform
//                Translate{
//                    dx:((idx)*step+animationValue)%distanceFromJoint
//                }
//                MatrixTransform{
//                    matrix: poseMatrix
//                }
//            }
//            components: [ruMesh,ruTransform]
//        }
//    }

}


