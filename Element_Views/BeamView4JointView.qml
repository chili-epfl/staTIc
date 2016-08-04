import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity{
    id:root
    property bool visible:  true
    property vector3d position;

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

    property real scaleFactor: Math.min(2*(Math.abs(axialForce)-minForce)/(maxForce-minForce) + 1,3)

    property int nModels: 5
    property matrix4x4 poseMatrix: Qt.matrix4x4()
    property int animationValue: axialForceType<0 ? (50-globalNumericAnimation) : globalNumericAnimation
    property int step: 10


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



    Entity{
        enabled: root.axialForceType!==0
        AnimationUnitDx{
            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement
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
            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement
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
            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement
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
            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement
            unitId: 3
            unitMesh: joint_commons.tiny_arrow_mesh
            step: root.step
            animationValue: root.animationValue
            module: 50
            direction: 1
            scaleFactor: root.scaleFactor
            rotate: root.axialForceType < 0
        }
        AnimationUnitDx{
            enabled: visible && settings.show_beam_axial_loads && !settings.show_displacement
            unitId: 4
            unitMesh: joint_commons.tiny_arrow_mesh
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



}
