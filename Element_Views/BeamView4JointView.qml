import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2

Entity{
    id:root
    property var backend_entity
    objectName: backend_entity ? backend_entity.objectName: ""
    property bool visible:  true
    property vector3d position;

    property bool non_default_visibility: true

    property vector3d extreme1 //Always the joint extreme
    property vector3d extreme2: backend_entity!==undefined ?(flip_extremes ?
                                   backend_entity.positionE1.times(staticsModule.modelScale) :
                                   backend_entity.positionE2.times(staticsModule.modelScale)): Qt.vector3d(0,0,0)
    property string extreme2_name:backend_entity!==undefined ? (flip_extremes ? backend_entity.e1Name : backend_entity.e2Name): "";
    /*Coarse representation */
    property int axialForceType: backend_entity!==undefined ? backend_entity.axialForceType:0 //-1 compression,0 nul, 1 tension
    property real axialForce : backend_entity!==undefined ? backend_entity.peakForces.x:0
    /*Fine one. Remeber these are in local reference system*/
    property real axialForce_extreme1: backend_entity!==undefined ? (flip_extremes ?
                                                                         backend_entity.forceE2.x :
                                                                         backend_entity.forceE1.x):0
    property real shearYForce_extreme1:backend_entity!==undefined ? (flip_extremes ?
                                                                         backend_entity.forceE2.y :
                                                                         backend_entity.forceE1.y):0
    property real shearZForce_extreme1:backend_entity!==undefined ? (flip_extremes ?
                                                                         backend_entity.forceE2.z :
                                                                         backend_entity.forceE1.z):0

    onAxialForce_extreme1Changed: lazyUpdate.start()
    onShearYForce_extreme1Changed: lazyUpdate.start()
    onShearZForce_extreme1Changed: lazyUpdate.start()

    property real axialForce_extreme2: backend_entity!==undefined ? (!flip_extremes ?
                                                                         backend_entity.forceE2.x :
                                                                         backend_entity.forceE1.x):0
    property real shearYForce_extreme2:backend_entity!==undefined ? (!flip_extremes ?
                                                                         backend_entity.forceE2.y :
                                                                         backend_entity.forceE1.y):0
    property real shearZForce_extreme2:backend_entity!==undefined ? (!flip_extremes ?
                                                                         backend_entity.forceE2.z :
                                                                         backend_entity.forceE1.z):0

    property bool flip_extremes: backend_entity!==undefined ? (extreme1.fuzzyEquals(backend_entity.positionE1.times(staticsModule.modelScale)) ?
                                     false :
                                     true) :false

    property vector3d globalForceExtreme1

    QQ2.Timer{
        id:lazyUpdate
        running: false
        interval: 300
        onTriggered: updateGlobalForceExtreme1()
    }

    function updateGlobalForceExtreme1(){
        var ab=extreme2.minus(extreme1).normalized();
        var a=Qt.vector3d(1,0,0);
        var b
        if(flip_extremes)
                b=extreme1.minus(extreme2).normalized();
        else
                b=extreme2.minus(extreme1).normalized();

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
        var shearY=result.times(Qt.vector3d(0,shearYForce_extreme1,0).times(-1))
        var shearZ=result.times(Qt.vector3d(0,0,shearZForce_extreme1).times(-1))
            if(flip_extremes){
                ab=ab.times(axialForce_extreme1);
            }
            else{
                ab=ab.times(axialForce_extreme1).times(-1);
            }

            globalForceExtreme1=ab.plus(shearY).plus(shearZ);

    }

    property real scaleFactor:maxForce==0 ? 0:Math.min(2*(Math.abs(axialForce)-minForce)/(maxForce-minForce) + 2,4)
    property int nModels: 5
    property matrix4x4 poseMatrix: Qt.matrix4x4()
    property int animationValue: axialForceType<0 ? (50-globalNumericAnimation) : globalNumericAnimation
    property int step: 5


//    QQ2.NumberAnimation on animationValue{
//            duration: 10000
//            from: axialForceType<0 ? 50 : 0
//            to: axialForceType<0 ? 0 : 50
//            loops: QQ2.Animation.Infinite
//            running: visible && axialForceType!==0
//    }

    onExtreme1Changed: computeTransform()
    onExtreme2Changed: computeTransform()
    onPositionChanged: computeTransform()
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
        result.m14=position.x
        result.m24=position.y
        result.m34=position.z
        poseMatrix=result;
    }

    function ssc(v){
        var matrix=Qt.matrix4x4(0, -v.z, v.y, 0,
                                v.z, 0, -v.x, 0,
                                -v.y, v.x, 0, 0,
                                0   ,   0, 0, 1)
        return matrix;
    }

//    PhongMaterial{
//        id:material_unitDX
//        shininess:0
////        StencilTest{
////            id:stencilTest
////            front.stencilFunction: StencilTestArguments.Equal
////            front.referenceValue: 255
////            front.comparisonMask: backend_entity!==undefined ? backend_entity.customID :0
////            back.stencilFunction: StencilTestArguments.Equal
////            back.referenceValue:  255
////            back.comparisonMask:  backend_entity!==undefined ? backend_entity.customID :0
////        }
//        DepthTest{
//            id:depthTest
//            depthFunction: DepthTest.LessOrEqual

//        }
//        QQ2.Component.onCompleted: {
//            if(backend_entity!==undefined){
//                for(var i=0;i<material_unitDX.effect.techniques.length;i++){
//                    var effect=material_unitDX.effect.techniques[i]
//                    effect.renderPasses[0].renderStates= depthTest
//                }
//            }
//        }
//        QQ2.Connections{
//            target: root
//            onBackend_entityChanged: if(backend_entity!==undefined){
//                                         for(var i=0;i<material_unitDX.effect.techniques.length;i++){
//                                             var effect=material_unitDX.effect.techniques[i]
//                                             effect.renderPasses[0].renderStates= depthTest
//                                         }
//                                     }
//        }
//    }


    Entity{
        enabled: root.axialForceType!==0
        AnimationUnitDx{
            enabled: parent.enabled && visible && settings.show_beam_axial_loads && !settings.show_displacement && non_default_visibility
            unitId: 0
            unitMesh: joint_commons.tiny_arrow_mesh
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }

        AnimationUnitDx{
            enabled: parent.enabled && visible && settings.show_beam_axial_loads && !settings.show_displacement && non_default_visibility
            unitId: 1
            unitMesh: joint_commons.tiny_arrow_mesh
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            enabled: parent.enabled && visible && settings.show_beam_axial_loads && !settings.show_displacement && non_default_visibility
            unitId: 2
            unitMesh: joint_commons.tiny_arrow_mesh
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            enabled: parent.enabled && visible && settings.show_beam_axial_loads && !settings.show_displacement && non_default_visibility
            unitId: 3
            unitMesh: joint_commons.tiny_arrow_mesh
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
//        AnimationUnitDx{
//            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement && non_default_visibility
//            unitId: 4
//            unitMesh: joint_commons.tiny_arrow_mesh
//            step: root.step
//            animationValue: root.animationValue
//            module: 50
//            direction: 1
//            scaleFactor: root.scaleFactor
//            rotate: root.axialForceType < 0
//        }

        Transform{
            id:transform
            matrix: poseMatrix
        }

        components: [transform]
    }



}
