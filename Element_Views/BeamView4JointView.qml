import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity{
    id:root
    property bool visible:  parent!=null ? parent.visible : false

    property vector3d extreme1 //Always the joint extreme
    property vector3d extreme2

    /*Coarse representation */
    property int axialForceType: 0 //-1 compression,0 nul, 1 tension
    property real axialForce : 0

    /*Fine one. Remeber these are in local reference system*/
    property real axialForce_extreme1
    property real shearYForce_extreme1
    property real shearZForce_extreme1
    onAxialForce_extreme1Changed: lazyUpdate.start()
    onShearYForce_extreme1Changed: lazyUpdate.start()
    onShearZForce_extreme1Changed: lazyUpdate.start()

    property real axialForce_extreme2
    property real shearYForce_extreme2
    property real shearZForce_extreme2
    property bool flip_extremes: false

    property vector3d globalForceExtreme1

    QQ2.Timer{
        id:lazyUpdate
        running: false
        interval: 300
        onTriggered: updateGlobalForceExtreme1()
    }

    function updateGlobalForceExtreme1(){
            var ab=extreme2.minus(extreme1).normalized();
            var shearY;
            var shearZ;
            if(flip_extremes){
                shearY=poseMatrix.times(Qt.vector3d(0,shearYForce_extreme1,0))
                shearZ=poseMatrix.times(Qt.vector3d(0,0,shearZForce_extreme1))
                ab=ab.times(axialForce_extreme1);
            }
            else{
                shearY=poseMatrix.times(Qt.vector3d(0,shearYForce_extreme1,0).times(-1))
                shearZ=poseMatrix.times(Qt.vector3d(0,0,shearZForce_extreme1).times(-1))
                ab=ab.times(axialForce_extreme1).times(-1);
            }
            globalForceExtreme1=ab.plus(shearY).plus(shearZ);
    }




    property real scaleFactor: 2*(Math.abs(axialForce)-minForce)/(maxForce-minForce) + 1


    property int nModels: 5
    property matrix4x4 poseMatrix: Qt.matrix4x4()
    property int animationValue: 0
    property int step: 10


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
        var result=Qt.matrix4x4();
        if(axb.x!==0 || axb.y!==0 || axb.z!==0){
            var tmp_ssc=ssc(axb);
            result=result.plus(tmp_ssc).plus(tmp_ssc.times(tmp_ssc).times((1-a.dotProduct(b))/Math.pow(axb.length(),2)));
            result.m44=1;
        }else{
            if(b.x===-1){
                result.m11=-1;
                result.m22=-1;
            }else{
                //Hack
                result.translate(0,0,0.00001);
            }
        }
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
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }

        AnimationUnitDx{
            unitId: 1
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 2
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 3
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            unitId: 4
            unitMesh: tiny_arrow
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }

        Transform{
            id:transform
            matrix: poseMatrix
        }

        components: [transform]
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
