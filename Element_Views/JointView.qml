import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.3 as QQ2

Entity{
    id:root
    readonly property string type: "joint"
    onEnabledChanged: {
        if(infobox.current_item == root)
            infobox.current_item=0;
    }
    property var connected_beams: []

    property bool visible:  true/*backgroundsubtraction.entropy < .10
                            && (
                               (applicationRoot.currentViewFilter=='JOINT'
                                   && (infobox.current_item == null || infobox.current_item == root))
                                || applicationRoot.currentViewFilter=='DESIGNER')
                            ? true : false*/

    property vector3d position
    property vector3d reaction
    property vector3d displacement
    property real displacementLimit: position.y==0 ? 1 : position.y/500
    property int displacementStatus: 0
    onDisplacementChanged: {
        if(displacement.x > displacementLimit ||
                displacement.z > displacementLimit){
            displacementStatus=2;
            settings.blink_displacement=2;
        }
        else if((displacement.x > displacementLimit/2 ||
                displacement.z > displacementLimit/2) && settings.blink_displacement<2){
            displacementStatus=1;
            settings.blink_displacement=1;
        }
        else
            displacementStatus=0;
        console.log(objectName,displacement)
    }

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
            running: visible && settings.show_joint && reactionMagnitude > 0
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
        enabled:  visible && settings.show_joint
    }
    PhongAlphaMaterial{
        id:material
        alpha: settings.show_displacement? 0.5 : 1
    }
    Transform{
        id:transform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(position.x,position.y,position.z);
            return m;
        }

    }
    components: [mesh,transform,material]

    Entity{
        Mesh{
            id:label_mesh
            enabled: visible && settings.show_joint
            source: root.objectName.length>0 ? "qrc:/UIMesh/3DObjects/"+root.objectName+".obj" :"qrc:/UIMesh/3DObjects/1.obj"
        }
        Transform{
            id:label_transform
            translation:Qt.vector3d(0,10,5)
            rotation:fromEulerAngles(0,-90,-90)/*quaternion_helper.invert(structure_tag.rotationQuaternion)*/
            scale: 2
        }
        PhongMaterial{
            id:label_material
            ambient:"#2C3539"

        }
        ObjectPicker {
            id:label_picker
            hoverEnabled: false
            onClicked: {
                sceneRoot.mouseEventHasBeenAccepted=true;
                if(settings.joint_is_selectable){
                    infobox.current_item=root
                }

            }
        }
        components: [label_mesh,label_transform,label_material,label_picker]

    }

    //Displacement entity
    Entity{
        enabled: visible && settings.show_joint && settings.show_displacement
        SphereMesh{
            id:displacement_mesh
            radius: 2
        }
        Transform{
            id:displacement_transform;
            translation:displacement
        }
        PhongMaterial{
            id:displacement_material
            ambient: {
                if(displacementStatus==2) return "red"
                if(displacementStatus==1) return "yellow"
                if(displacementStatus==0) return "green"
            }
        }
        components: [displacement_mesh,displacement_transform,displacement_material]
    }


    Mesh{
        id:tiny_arrow
        enabled: visible && settings.show_beam_axial_loads && reactionMagnitude>0
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
            isReaction: true
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
            isReaction: true

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
            isReaction: true

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
            isReaction: true

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
            isReaction: true
        }



        components: [
            Transform{
                matrix: poseMatrix
            }]
    }


}


