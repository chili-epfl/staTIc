import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

import QtQuick 2.0 as QQ2
import QtPhysics.unofficial 1.0

Entity{
    id:rootEntity

    readonly property string type: "beam"

    property string materialID

    onMaterialIDChanged: apply_animation.start()

    property vector3d extreme1
    property vector3d extreme2

    onExtreme1Changed: computeTransform()
    onExtreme2Changed: computeTransform()

    property real length: extreme1.minus(extreme2).length()

    /*The segments for the deforming beam*/
    property var segments

    property size beamSize    
    onBeamSizeChanged: apply_animation.start();

    property size realBeamSize
    property size tangibleSection: "34x17"
    property int axialForceType: 0 //-1 compression,0 nul, 1 tension

    /*The stress relative to th esize and material.
     *If it's 1 or more, it is above the limits*/
    property real relativeAxialStress: 0
    onRelativeAxialStressChanged: console.log(rootEntity.objectName+":"+relativeAxialStress)
    property matrix4x4 poseMatrix

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

    /*Main mesh*/

    CylinderMesh{
        id:mainMesh
        radius: 1
        length: 25
        enabled:  applicationRoot.currentViewFilter=='DESIGNER'
                  && infobox.current_item == rootEntity
                  && backgroundsubtraction.entropy < .10
                  ? true : false
    }

    QQ2.NumberAnimation{
        id: apply_animation
        target:  main_mesh_material
        property: "alpha"
        from: 1
        to:0.75
        running: false
    }

    PhongAlphaMaterial{
        id:main_mesh_material
        ambient: infobox.current_item == rootEntity ? "green" : "grey"
        diffuse:"grey"
        specular:"black"
        alpha:0.75
    }

    Transform{
        id:transform
        matrix: poseMatrix
    }

    components: [transform]


    Entity{

        property Transform transform: Transform{
                translation:Qt.vector3d(0,0.5*mainMesh.length+1.5*overview_mesh.radius,0)
        }
        components: [mainMesh,this.transform,main_mesh_material]
    }
    Entity{

        property Transform transform: Transform{
                translation:Qt.vector3d(0,-0.5*mainMesh.length-1.5*overview_mesh.radius,0)
        }
        components: [mainMesh,this.transform,main_mesh_material]
    }



    /*Overview related part*/
    Entity{
        id:overview_entity
        enabled: backgroundsubtraction.entropy < .10
                 && (
                    (applicationRoot.currentViewFilter=='BEAM'
                        && (infobox.current_item == null || infobox.current_item == rootEntity))
                     || applicationRoot.currentViewFilter=='DESIGNER')
                 ? true : false
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
            property real delta: 0.8*Math.min(relativeAxialStress,1)
            scale3D:Qt.vector3d(1-axialForceType*(delta),1+axialForceType*(delta),1-axialForceType*(delta));
            QQ2.Behavior on scale3D{
                QQ2.Vector3dAnimation{
                    duration: 500
                }
            }
        }
        components: [overview_material,overview_mesh,overview_scale]
    }

    /*-----Reference bodies----*/
    Entity{
        components: [
            SphereMesh{
                id:extreme1Ref
                radius: 5
                enabled: overview_entity.enabled &&
                         applicationRoot.currentViewFilter=='BEAM' &&
                         infobox.current_item == rootEntity ?
                         true: false
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
                enabled: overview_entity.enabled &&
                         applicationRoot.currentViewFilter=='BEAM' &&
                         infobox.current_item == rootEntity ?
                         true: false
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
                enabled: overview_entity.enabled &&
                         applicationRoot.currentViewFilter=='BEAM' &&
                         infobox.current_item == rootEntity ?
                         true: false
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


    /*-----Physical body----*/
    Entity{
       id:physicBody
       objectName: rootEntity.objectName
       CuboidMesh{
           enabled: true
           id:pBodyMesh
           xExtent: 30
           zExtent: 30
           yExtent: rootEntity.length

       }
//       CylinderMesh{
//           enabled: true
//           id:pBodyMesh
//           radius: 15
//           length: rootEntity.length
//       }
       PhysicsBodyInfo{
           id:pBody
           kinematic:true
           mass: 10
           friction: 500
           restitution: 0
       }
       PhongAlphaMaterial{
           id:transparentMaterial
           alpha:0.0
       }
       components: [pBodyMesh,pBody,transparentMaterial]

    }

    /*Pickers and Dragging anchors*/
    property bool drag_anchor_enabled:false;
    property vector3d current_anchor_position: Qt.vector3d(0,0,0);
//    SphereMesh{
//        id:drag_mesh
//        radius:10
//    }
    CylinderMesh{
        id:drag_mesh
        radius: 5
        length: 40
    }
    PhongAlphaMaterial{
        id:drag_material
        ambient: "black"
        diffuse: "black"
        specular: "black"
        shininess: 0
        alpha:0.00
    }
    Entity{
        enabled: applicationRoot.currentViewFilter=='BEAM' ||
                 applicationRoot.currentViewFilter=='DESIGNER' ? true : false
        property Transform transform: Transform{
            rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
        }
        property ObjectPicker objectPicker:ObjectPicker{
            hoverEnabled: drag_anchor_enabled
            onEntered: {current_anchor_position=Qt.vector3d(0,0,0);
                        }
            onClicked: {
                if(parent.enabled)
                    infobox.current_item=rootEntity
            }
        }
        components: [drag_mesh,drag_material,this.transform,objectPicker]
    }
    NodeInstantiator {
        model: (length-40)/(4*(drag_mesh.radius+2))-1;
        delegate:Entity{
            enabled: applicationRoot.currentViewFilter=='BEAM' ||
                     applicationRoot.currentViewFilter=='DESIGNER' ? true : false
            property Transform transform: Transform{
                rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                translation:Qt.vector3d(0,(index+1)*(2*(drag_mesh.radius+2)),0)
            }
            property ObjectPicker objectPicker:ObjectPicker{
                hoverEnabled: drag_anchor_enabled
                onEntered: current_anchor_position=transform.translation;
//                onClicked: {
//                        if(parent.enabled)
//                            infobox.current_item=rootEntity
//                }
            }
            components: drag_anchor_enabled ? [drag_mesh,drag_material,transform,objectPicker] : []
        }
    }
    NodeInstantiator {
        model: (length-40)/(4*(drag_mesh.radius+2))-1;
        delegate:Entity{
            enabled: applicationRoot.currentViewFilter=='BEAM' ||
                     applicationRoot.currentViewFilter=='DESIGNER' ? true : false
             property Transform transform: Transform{
                id:drag_transform
                rotation: fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                translation:Qt.vector3d(0,-(index+1)*(2*(drag_mesh.radius+2)),0)
            }
            property ObjectPicker objectPicker: ObjectPicker{
                hoverEnabled: drag_anchor_enabled
                onEntered: current_anchor_position=transform.translation
//                onClicked: {
//                    if(parent.enabled)
//                        infobox.current_item=rootEntity
//                }
            }
            components: drag_anchor_enabled ? [drag_mesh,drag_material,transform,objectPicker] : []

        }
    }


}
