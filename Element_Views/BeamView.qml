import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import QtQuick 2.0 as QQ2
import QtPhysics.unofficial 1.0
import DeformingBeamMesh 1.0

import "qrc:/ui/UI"
import "qrc:/opengl/Opengl"
Entity{
    id:rootEntity
    property var backend_entity
    objectName: backend_entity.objectName
    property bool disable_beam_selection_for_load: false
//    property int stencilValue: backend_entity.customID

//    StencilTest{
//        id:stencilTest
//        front.stencilFunction: StencilTestArguments.Equal
//        front.referenceValue: 255
//        front.comparisonMask: stencilValue
//        back.stencilFunction: StencilTestArguments.Equal
//        back.referenceValue: 255
//        back.comparisonMask: stencilValue
//    }


    onEnabledChanged: {
        if(infobox.current_item == rootEntity)
            infobox.current_item=0;
    }
    readonly property string type: "beam"

    property string materialID:backend_entity.materialID

    function setMaterialID(materialID){
        backend_entity.materialID=materialID;
    }

    onMaterialIDChanged: apply_animation.start()

    property vector3d extreme1:backend_entity.positionE1.times(staticsModule.modelScale)
    property vector3d extreme2:backend_entity.positionE2.times(staticsModule.modelScale)

    property string extreme1_name:backend_entity.e1Name;
    property string extreme2_name:backend_entity.e2Name;

    onExtreme1Changed: computeTransform()
    onExtreme2Changed: computeTransform()


    property real length: extreme1.minus(extreme2).length()

    /*The segments for the deforming beam*/
    property var segments:backend_entity.segments

    property size beamSize:Qt.size(backend_entity.size.width*staticsModule.modelScale,backend_entity.size.height*staticsModule.modelScale)
    onBeamSizeChanged: {apply_animation.start();}

    property size realBeamSize:backend_entity.size

    function setBeamSize(size){
        backend_entity.size=size;
    }

    property size tangibleSection: backend_entity.tangibleSection

    property int axialForceType: backend_entity.axialForceType //-1 compression,0 nul, 1 tension

    /*The stress relative to the size and material.
     *If it's 1 or more, it is above the limits*/
    property real relativeAxialStress: backend_entity.relativeAxialStress
    onRelativeAxialStressChanged:{
            if(relativeAxialStress>=0.8)
                settings.blink_stress=2;
            else if(relativeAxialStress>=0.5)
                settings.blink_stress=1;
    }

    property real dispY:backend_entity.peakDisplacement.y

    property real dispZ:backend_entity.peakDisplacement.z
    property real relativeDisplacementY:backend_entity.relativePeakDisplacement.y
    property real relativeDisplacementZ:backend_entity.relativePeakDisplacement.z

    property vector3d extreme1Displacement:backend_entity.displacementE1.times(staticsModule.modelScale);
    property vector3d extreme2Displacement:backend_entity.displacementE2.times(staticsModule.modelScale);


//    onExtreme1DisplacementChanged: computeTransform4Displacement();
//    onExtreme2DisplacementChanged: computeTransform4Displacement();

    property matrix4x4 poseMatrix
    property quaternion quaternionTest;
    property vector3d translationTest;

    property bool enable_deformation:settings.show_displacement

    Entity{
        enabled: settings.show_stress
        PercNumberEntity{
            enabled: parent.enabled
            id:number_entity
            number:relativeAxialStress
            QQ2.Behavior on number{
                QQ2.NumberAnimation{
                    duration:2000
                }
            }
            color:PhongMaterial{
                  diffuse:Qt.hsla(Math.max(0.33*(1-number_entity.number),0),1,0.5)
            }
        }
        Transform{
            id:text_transform
            translation:enable_deformation? Qt.vector3d(0,0,20+tangibleSection.width) : Qt.vector3d(0,0,20)
            rotation:quaternion_helper.invert(quaternionTest)
            scale: 10
        }
        components:[text_transform]
    }

    function computeTransform(){
        var b=extreme2.minus(extreme1).normalized();
        quaternionTest=transform.fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
        if(b.x!=1 && b.x!=-1){
            var axis=Qt.vector3d(1,0,0).crossProduct(b).normalized();
            var angle=Math.acos(Qt.vector3d(1,0,0).dotProduct(b));
            quaternionTest=quaternion_helper.product(transform.fromAxisAndAngle(axis, angle*180.0/Math.PI),quaternionTest)
        }
        var center=extreme1.plus(extreme2).times(0.5);
        translationTest.x=center.x;
        translationTest.y=center.y;
        translationTest.z=center.z;
    }

    function ssc(v){
        var matrix=Qt.matrix4x4(0, -v.z, v.y, 0,
                                v.z, 0, -v.x, 0,
                                -v.y, v.x, 0, 0,
                                0   ,   0, 0, 1)
        return matrix;
    }

    /*Main mesh*/

    QQ2.NumberAnimation{
        id: apply_animation
//        target:  main_mesh_material
//        property: "alpha"
        from: 1
        to:0.75
        running: false
    }

    PhongMaterial{
        id:main_mesh_material
        ambient: infobox.current_item == rootEntity ? "green" : "grey"
        diffuse:"grey"
        specular:"black"
//        QQ2.Component.onCompleted: {
//            for(var i=0;i<main_mesh_material.effect.techniques.length;i++){
//                var effect=main_mesh_material.effect.techniques[i]
//                effect.renderPasses[0].renderStates=stencilTest
//            }
//        }
        //alpha:0.75
    }

    Transform{
        id:transform
        rotation:quaternionTest
        translation:translationTest
    }

    components: [transform]

    property real main_mesh_lenght: settings.show_beam_axial_loads ? Math.max(rootEntity.length/2-50,5) : Math.max(rootEntity.length/2-25,5)
    property real available_length: rootEntity.length-50
    property real space_from_spring_and_arrows: 63
    Entity{
        //Left rod
        enabled: settings.show_beam && !settings.show_displacement && !enable_deformation && available_length>space_from_spring_and_arrows
        property Transform transform: Transform{
                scale3D:  Qt.vector3d(1,0.5*(available_length-space_from_spring_and_arrows)/beam_commons.main_mesh_cylinder.length,1)
                translation://length > (2* main_mesh_lenght + 30) ?
                                Qt.vector3d(0,0.5*0.5*(available_length-space_from_spring_and_arrows)+10,0)
                              //:Qt.vector3d(0,0,0)
        }
        components: [beam_commons.main_mesh_cylinder,this.transform,main_mesh_material]
    }
    Entity{
        //Right rod
        enabled: settings.show_beam && !settings.show_displacement&& !enable_deformation && available_length>space_from_spring_and_arrows// && length > (2*main_mesh_lenght + 30)
        property Transform transform: Transform{
                scale3D:  Qt.vector3d(1,0.5*(available_length-space_from_spring_and_arrows)/beam_commons.main_mesh_cylinder.length,1)
                translation:Qt.vector3d(0,-0.5*0.5*(available_length-space_from_spring_and_arrows)-10,0)
        }
        components: [beam_commons.main_mesh_cylinder,this.transform,main_mesh_material]
    }

    Entity{
        enabled: !settings.show_displacement && !enable_deformation
        property Transform transform: Transform{
                scale3D:  Qt.vector3d(rootEntity.tangibleSection.height/2,rootEntity.length/2,rootEntity.tangibleSection.width/2)
        }
        TransparentMaterial{
            id: transparentMaterial
//            stencilValue: rootEntity.stencilValue

        }
        components: [beam_commons.cube_mesh_inv_normals,transparentMaterial,this.transform]
    }

    Entity{
        //Sphere mesh
        enabled: settings.show_beam_sphere && !enable_deformation
        components: [
            beam_commons.sphere_mesh,
            main_mesh_material
        ]
    }

    Entity{
        //Spring mesh
        id:spring_entity
        enabled: settings.show_beam_spring && !settings.show_displacement && !enable_deformation
        property real scaleFactor:Math.min(0.75,tangibleSection.width/22,tangibleSection.height/22,33*available_length/space_from_spring_and_arrows)
        property Transform transform:Transform{
            //z:width,x:height,y:lenght
            //scale3D:Qt.vector3d(17/9,1,35/9)
            //22 is the size of the spring
            scale3D: axialForceType > 0 ? Qt.vector3d(spring_entity.scaleFactor,spring_entity.scaleFactor + Math.min(relativeAxialStress,1),spring_entity.scaleFactor) : Qt.vector3d(spring_entity.scaleFactor,spring_entity.scaleFactor -0.55* Math.min(relativeAxialStress,1),spring_entity.scaleFactor)
            QQ2.Behavior on scale3D{
                QQ2.Vector3dAnimation{
                    duration: 500
                }
            }
        }

        components: available_length<=space_from_spring_and_arrows && infobox.current_item == rootEntity ?
                        [beam_commons.spring_mesh, this.transform, main_mesh_material] :
                        [beam_commons.spring_mesh, this.transform, beam_commons.phong_grey]
    }

    Entity{
        enabled:  false
        property Transform transform: Transform{
            translation:Qt.vector3d(20,0,0)
        }
        components: [beam_commons.sphere_mesh_reference,beam_commons.phong_blue_reference,this.transform ]
    }


    /*-----Physical body----*/
    Entity{
       id:physicBody
       objectName: rootEntity.objectName
       CuboidMesh{
           enabled: true
           id:pBodyMesh
           xExtent: 17
           zExtent: 30
           yExtent: rootEntity.length
       }
       PhysicsBodyInfo{
           id:pBody
           kinematic:true
           mass: 10
           friction: 500
           restitution: 0
           mask: 2;
           group: 1;
       }
       components: [pBodyMesh,pBody]

    }




//    /*Displacement entity*/
//    property quaternion displacementQuaternion;
//    property vector3d displacementTranslation;
//    property real displacementMeshLenght;

//    QQ2.Connections{
//        target: settings
//        onExagerate_displacement_factorChanged:computeTransform4Displacement()
//    }

//    function computeTransform4Displacement(){
//        var e1=extreme1.plus(extreme1Displacement.times(settings.exagerate_displacement_factor))
//        var e2=extreme2.plus(extreme2Displacement.times(settings.exagerate_displacement_factor))
//        displacementMeshLenght=e2.minus(e1).length();
//        var b=e2.minus(e1).normalized();
//        displacementQuaternion=displacement_transform.fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
//        if(b.x!=1 && b.x!=-1){
//            var axis=Qt.vector3d(1,0,0).crossProduct(b).normalized();
//            var angle=Math.acos(Qt.vector3d(1,0,0).dotProduct(b));
//            displacementQuaternion=quaternion_helper.product(displacement_transform.fromAxisAndAngle(axis, angle*180.0/Math.PI),displacementQuaternion)
//        }
//        var center=e2.plus(e1).timebeam_commons.transparent_materials(0.5);
//        displacementTranslation.x=center.x;
//        displacementTranslation.y=center.y;
//        displacementTranslation.z=center.z;
//    }

//    Entity{
//        enabled: settings.show_displacement
//        components: [Transform{
//                rotation:quaternion_helper.invert(quaternionTest)
//                }
//        ]

//        Entity{
//            components: [Transform{
//                translation:translationTest.times(-1)
//            }]
//            Entity{
//                Transform{
//                    id:displacement_transform
//                    scale3D:Qt.vector3d(1,(displacementMeshLenght-50)/beam_commons.dispalcement_mesh.length,1)
//                    rotation:displacementQuaternion
//                    translation:displacementTranslation
//                }
//                components: [displacement_transform,beam_commons.dispalcement_mesh,beam_commons.phong_material_green]
//            }
//        }


//    }

    /*Deforming Mesh*/

    Entity{
        id:deforming_entity
        enabled: enable_deformation
        property real deforming_length:Math.max(10,rootEntity.length-60)
        Transform{
            id:deforming_transform
            rotation:transform.fromAxisAndAngle(Qt.vector3d(0, 0, 1), -90)
            translation: Qt.vector3d(0,deforming_entity.deforming_length/2,0)
        }
        DeformingBeamMesh{
            id:deforming_mesh
            offset:Qt.vector2d(30,30)
            segments: 30
            keyframes: 1
            exagerate:settings.exagerate_displacement_factor*staticsModule.modelScale;
            displacements:rootEntity.segments
            length:deforming_entity.deforming_length
            size:tangibleSection
        }

        components: infobox.current_item == rootEntity ?
                        [deforming_mesh,deforming_transform,main_mesh_material]
                      :
                        [deforming_mesh,deforming_transform,beam_commons.deformingMeshMaterial]
    }


    /*Pickers and Dragging anchors*/

//    SphereMesh{
//        id:drag_mesh
//        radius:10
//    }

    Entity{
        enabled: settings.beam_is_selectable && infobox.current_item!=rootEntity
        property Transform transform: Transform{
            scale3D:  Qt.vector3d(2,available_length/beam_commons.main_mesh_cylinder.length,2)

        }
        property ObjectPicker objectPicker:ObjectPicker{

            onClicked: {
                sceneRoot.mouseEventHasBeenAccepted=true;
                if(parent.enabled && infobox.current_item!=rootEntity && !disable_beam_selection_for_load){
                    infobox.current_item=rootEntity
                }
            }
        }
        property CylinderMesh clicking_mesh: CylinderMesh{
            radius: 5
            length: rootEntity.length-20
        }
        components: infobox.current_item!==rootEntity && !disable_beam_selection_for_load ?
                        [beam_commons.main_mesh_cylinder,this.transform,objectPicker]:
                        [beam_commons.main_mesh_cylinder,this.transform]
    }

//    NodeInstantiator {
//        model:
//        delegate:Entity{
//            enabled: settings.beam_is_selectable
//            property Transform transform: Transform{
//                rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
//                translation:Qt.vector3d(beam_commons.drag_mesh.radius+tangibleSection.height/2,(index+1)*(2*(beam_commons.drag_mesh.radius+2)),0)
//            }
//            property ObjectPicker objectPicker:ObjectPicker{
//                onClicked: {
//                    sceneRoot.mouseEventHasBeenAccepted=true;
//                    current_anchor_position=Qt.vector3d(0,transform.translation.y,0);
//                }
//            }
//            components: drag_anchor_enabled ? [beam_commons.drag_mesh,beam_commons.transparent_material,transform,objectPicker] : []
//            Entity{
//                enabled: drag_anchor_enabled
//                property Transform transform: Transform{
//                   scale3D:Qt.vector3d(0.5,0.5,0.5)
//               }
//               components: [this.transform,beam_commons.drag_mesh,beam_commons.phong_material_green]

//            }
//        }
//    }
//    NodeInstantiator {
//        model: (length-40)/(4*(beam_commons.drag_mesh.radius+2))-1;
//        delegate:Entity{
//            enabled: settings.beam_is_selectable
//             property Transform transform: Transform{
//                id:drag_transform
//                rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
//                translation:Qt.vector3d(beam_commons.drag_mesh.radius+tangibleSection.height/2,-(index+1)*(2*(beam_commons.drag_mesh.radius+2)),0)
//            }
//            property ObjectPicker objectPicker: ObjectPicker{
//                onClicked: {
//                    sceneRoot.mouseEventHasBeenAccepted=true;
//                    current_anchor_position=Qt.vector3d(0,transform.translation.y,0);
//                }
//            }

//            components: drag_anchor_enabled ? [beam_commons.drag_mesh,beam_commons.transparent_material,transform,objectPicker] : []
//            Entity{
//                enabled: drag_anchor_enabled
//                property Transform transform: Transform{
//                   scale3D:Qt.vector3d(0.5,0.5,0.5)
//               }
//               components: [this.transform,beam_commons.drag_mesh,beam_commons.phong_material_green]

//            }
//        }
//    }


}
