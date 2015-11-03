import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.0 as QQ2
import QtPhysics.unofficial 1.0
Entity{
    id:rootEntity
    property bool visible:  applicationRoot.currentViewFilter=='BEAM' ? true : false

    property vector3d extreme1
    property vector3d extreme2

    property real length: extreme1.minus(extreme2).length()

    property int axialForceType: 0 //-1 compression,0 nul, 1 tension
    property real axialForce : 0

    onAxialForceTypeChanged: {
        animation.stop();
        var prevAnimationValue=animationValue
        if(axialForceType>0){
            animation.from=prevAnimationValue
            animation.to=length/2-step+prevAnimationValue
        }
        else{
            animation.to=prevAnimationValue
            animation.from=length/2-step+prevAnimationValue
        }
        animation.restart()
    }

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

    property int nModels: 20
    property matrix4x4 poseMatrix

    property int animationValue: 0
    property int step: 10
    property int forceRadius: forceRadii["small"]

    property var forceRadii: { "small": 1, "medium": 2, "big":3 }

    QQ2.NumberAnimation on animationValue{
        id:animation
        duration: 10000
        from: 0
        to:  length/2-step
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
        enabled:  visible
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
       components: [pBodyMesh,pBody]
    }


    //First sequence

    Entity{
        components: [SphereMesh{
            enabled: !visible || 0 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((0)*step+animationValue)%Math.round(length/2-step)+step)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 1 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((1)*step+animationValue)%Math.round(length/2-step)+step)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible ||2 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((2)*step+animationValue)%Math.round(length/2-step)+step)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 3 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((3)*step+animationValue)%Math.round(length/2-step)+step)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 4 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((4)*step+animationValue)%Math.round(length/2-step)+step)
            }
        }]
    }


    //Second Sequence
    Entity{
        components: [SphereMesh{
            enabled: !visible || 0 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((0)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 1 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((1)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible ||2 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((2)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 3 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((3)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
            }
        }]
    }
    Entity{
        components: [SphereMesh{
            enabled: !visible || 4 > Math.round(length/2*step) ? false : true
            radius: forceRadius;
        }
        , Transform{
            Translate{
                dy:(((4)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
            }
        }]
    }



//        NodeInstantiator{
//            active:visible
//            model:nModels
//            //asynchronous:true
//            delegate:
//                Entity{
//                id:animationUnit
//                property int idx: index
//                SphereMesh{
//                    enabled: !visible || idx > Math.round(length/2*step) ? false : true
//                    id:auMesh
//                    radius: forceRadius;
//                }
//                Transform{
//                    id:auTransform
//                    Translate{
//                        dy:(((idx)*step+animationValue)%Math.round(length/2-step)+step)
//                    }
//                 }
//                components: [auMesh,auTransform]
//            }
//        }

//        NodeInstantiator{
//            model:nModels
//            //asynchronous:true
//            delegate:
//                Entity{
//                id:animationUnit2
//                property int idx: index
//                SphereMesh{
//                    enabled: !visible || idx > Math.round(length/(2*step)) ? false : true
//                    id:auMesh2
//                    radius: forceRadius;
//                }
//                Transform{
//                    id:auTransform2
//                    Translate{
//                        dy:(((idx)*step+animationValue)%Math.round(length/2-step)+step)*(-1)
//                    }
//                }
//                components: [auMesh2,auTransform2]
//            }
//        }

}
