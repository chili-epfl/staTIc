import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Logic 2.0
import QtQuick 2.0 as QQ2
import "qrc:/opengl/Opengl/"
import "qrc:/element_views/Element_Views/"

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 60
        nearPlane : 0.1
        farPlane : 10000
        position: Qt.vector3d( 0.0, 0, 60)//Math.max(Math.max(z_current_force_mesh.length,100),0.8*Math.max(x_current_force_mesh.length,y_current_force_mesh.length)/Math.tan(fieldOfView/2)))
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }


    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "#8cb5f5"
            }
        }
    ]

    CameraController{
        camera: camera
    }

    function resetCamera(){
        camera.position=Qt.vector3d( 0.0, 0, 60)
        camera.upVector= Qt.vector3d( 0.0, 1.0, 0.0 )
    }


    property vector3d currentForce: Qt.vector3d(0,0,0)

    property real maxForceLength: 1

    property real equilibrium_distance: currentForce.length()/maxForceLength;

    property int scaleFactor: (Math.log(maxForceLength)/Math.LN10) > 1 ?
                               Math.pow(10,Math.log(maxForceLength)/Math.LN10-1)
                              :
                              1

    CylinderMesh{
        id:axis_mesh
        length: 20
        radius: 0.5
    }
    PhongMaterial{
        id:red
        ambient:"#980000"
        diffuse:"black"
        specular:"black"
        shininess:0
    }
    PhongMaterial{
        id:blue
        ambient:"#001a98"
        diffuse:"black"
        specular:"black"
    }
    PhongMaterial{
        id:green
        ambient:"#479800"
        diffuse:"black"
        specular:"black"
        shininess:0
    }
    PhongMaterial{
        id:grey
        ambient:"grey"
        diffuse:"black"
        specular:"black"
        shininess:0
    }
    Transform{
        id:x_axis_transform
        rotation: fromAxisAndAngle(Qt.vector3d(0,0,1), -90)
        translation:Qt.vector3d(0.5*axis_mesh.length,0,0)
    }
    Transform{
        id:y_axis_transform
        translation:Qt.vector3d(0,0.5*axis_mesh.length,0)
    }
    Transform{
        id:z_axis_transform
        rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
        translation:Qt.vector3d(0,0,0.5*axis_mesh.length)
    }
    Transform{
        id:x_axis_transform_neg
        rotation: fromAxisAndAngle(Qt.vector3d(0,0,1), -90)
        translation:Qt.vector3d(-0.5*axis_mesh.length,0,0)
    }
    Transform{
        id:y_axis_transform_neg
        translation:Qt.vector3d(0,-0.5*axis_mesh.length,0)
    }
    Transform{
        id:z_axis_transform_neg
        rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
        translation:Qt.vector3d(0,0,-0.5*axis_mesh.length)
    }
    Mesh{
        id:axis_arrow_mesh
        source:"qrc:/ui/UI/arrowR0_70L1.obj"
    }
    Transform{
        id:axis_arrow_transform
        scale:1.25
        translation:Qt.vector3d(0,0.5*axis_mesh.length,0)
    }
    Mesh{
        id:force_arrow_mesh
        source:"qrc:/ui/UI/arrowR0_70L0_5.obj"
    }

    /*Current Force Entity*/
    Entity{

        PhongMaterial{
            id:color_current_force
            property real h:0.33-0.33*equilibrium_distance
            ambient:Qt.hsla(h,1,0.32)
            diffuse:"grey"
            specular:"#000000"
            shininess:0

            QQ2.Behavior on h {
                QQ2.NumberAnimation{duration: 1000}
            }

        }

        Entity{
            objectName: "x_axis"
            components: [axis_mesh,x_axis_transform,red]
            Entity{
                components: [ axis_arrow_mesh ,axis_arrow_transform,red]
            }
        }
        Entity{
            objectName: "x_axis_neg"
            components: [axis_mesh,x_axis_transform_neg,grey]
        }
        Entity{
            objectName: "y_axis"
            components: [axis_mesh,y_axis_transform,green]
            Entity{
                components: [ axis_arrow_mesh ,axis_arrow_transform,green]
            }

        }
        Entity{
            objectName: "y_axis_neg"
            components: [axis_mesh,y_axis_transform_neg,grey]

        }
        Entity{
            objectName:"z_axis"
            components: [axis_mesh,z_axis_transform,blue]
            Entity{
                components: [ axis_arrow_mesh ,axis_arrow_transform,blue]
            }

        }
        Entity{
            objectName:"z_axis_neg"
            components: [axis_mesh,z_axis_transform_neg,grey]

        }

        Entity{
            SphereMesh{
                id:centre_mesh
                radius: axis_mesh.radius*5
            }
            components: [centre_mesh,color_current_force]
        }

        Entity{
            LogicComponent {
                id:x_current_force_logic
                onFrameUpdate: {
                    if( x_current_force_mesh.length<0.0001)
                        x_current_force_transform.force_dir=0;
                    if(Math.abs(currentForce.x) > 0.0001 && x_current_force_transform.force_dir === 0){
                        if(currentForce.x>0){
                            x_current_force_transform.force_dir=1;
                        }
                        else{
                            x_current_force_transform.force_dir=-1;
                        }
                        x_current_force_mesh.length=Qt.binding(function(){
                            if(Math.abs(currentForce.x/scaleFactor) > 0.0001)
                                return Math.abs(currentForce.x/scaleFactor)
                            else return 0;
                            });
                    }
                    else if(Math.abs(currentForce.x)>0.0001 && x_current_force_transform.force_dir * currentForce.x < 0){
                        x_current_force_mesh.length=0
                    }
                }
            }

            CylinderMesh{
                id:x_current_force_mesh
                radius: axis_mesh.radius*3
                length: 0
                QQ2.Behavior on length {
                    QQ2.NumberAnimation{duration:750}
                }
            }
            Transform{
                id:x_current_force_transform
                property int force_dir:0
                rotation: fromAxisAndAngle(Qt.vector3d(0,0,1), -90)
                translation:Qt.vector3d(force_dir*(0.99*centre_mesh.radius+0.5*x_current_force_mesh.length),0,0)
            }
            components: [color_current_force,x_current_force_mesh,x_current_force_transform,x_current_force_logic]
//            Entity{
//                enabled: x_current_force_mesh.length > 0.0001 ? true:false
//                Transform{
//                    id:x_force_arrow_transform
//                    scale:3
//                    translation:Qt.vector3d(0,0.5*x_current_force_mesh.length,0)
//                }
//                components: [force_arrow_mesh,x_force_arrow_transform,color_current_force]
//            }
        }
        Entity{

            LogicComponent {
                id:y_current_force_logic
                onFrameUpdate: {
                    if( y_current_force_mesh.length<0.0001)
                        y_current_force_transform.force_dir=0;
                    if(Math.abs(currentForce.y) > 0.0001 && y_current_force_transform.force_dir === 0){
                        if(currentForce.y>0){
                            y_current_force_transform.force_dir=1;
                        }
                        else{
                            y_current_force_transform.force_dir=-1;
                        }
                        y_current_force_mesh.length=Qt.binding(function(){
                            if(Math.abs(currentForce.y/scaleFactor) > 0.0001)
                                return Math.abs(currentForce.y/scaleFactor)
                            else return 0;
                            });
                    }
                    else if(Math.abs(currentForce.y)>0.0001 && y_current_force_transform.force_dir * currentForce.y < 0){
                        y_current_force_mesh.length=0
                    }
                }
            }

            CylinderMesh{
                id:y_current_force_mesh
                radius: axis_mesh.radius*3
                length:0

                QQ2.Behavior on length {
                    QQ2.NumberAnimation{
                        id:y_current_force_animation
                        duration:750
                    }
                }
            }
            Transform{
                id:y_current_force_transform
                property int force_dir:0
                translation:Qt.vector3d(0,force_dir*(0.99*centre_mesh.radius+0.5*y_current_force_mesh.length),0)

            }
            components: [color_current_force,y_current_force_mesh,y_current_force_transform, y_current_force_logic]
//            Entity{
//                enabled: y_current_force_mesh.length > 0.0001 ? true:false
//                Transform{
//                    id:y_force_arrow_transform
//                    scale:3
//                    translation:Qt.vector3d(0,0.5*y_current_force_mesh.length,0)
//                }
//                components: [force_arrow_mesh,y_force_arrow_transform,color_current_force]
//            }
        }
        Entity{
            LogicComponent {
                id:z_current_force_logic
                onFrameUpdate: {
                    if( z_current_force_mesh.length<0.0001)
                        z_current_force_transform.force_dir=0;
                    if(Math.abs(currentForce.z) > 0.0001 && z_current_force_transform.force_dir === 0){
                        if(currentForce.z>0){
                            z_current_force_transform.force_dir=1;
                        }
                        else{
                            z_current_force_transform.force_dir=-1;
                        }
                        z_current_force_mesh.length=Qt.binding(function(){
                            if(Math.abs(currentForce.z/scaleFactor) > 0.0001)
                                return Math.abs(currentForce.z/scaleFactor)
                            else return 0;
                            });
                    }
                    else if(Math.abs(currentForce.z)>0.0001 && z_current_force_transform.force_dir * currentForce.z < 0){
                        z_current_force_mesh.length=0
                    }
                }
            }


            CylinderMesh{
                id:z_current_force_mesh
                radius: axis_mesh.radius*3
                length: 0
                QQ2.Behavior on length {
                    QQ2.NumberAnimation{
                        duration:750
                    }
                }
            }
            Transform{
                id:z_current_force_transform
                property int force_dir:0
                rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
                translation:Qt.vector3d(0,0,force_dir*(0.99*centre_mesh.radius+0.5*z_current_force_mesh.length))
                onForce_dirChanged: console.log(force_dir)
            }
            components: [color_current_force,z_current_force_mesh,z_current_force_transform,z_current_force_logic]
//            Entity{
//                enabled: z_current_force_mesh.length > 0.0001 ? true:false
//                Transform{
//                    id:z_force_arrow_transform
//                    scale:3
//                    translation:Qt.vector3d(0,0.5*z_current_force_mesh.length,0)
//                }
//                components: [force_arrow_mesh,z_force_arrow_transform,color_current_force]
//            }
        }
    }

//    /*Upcoming Force*/
//    Entity{

//        Entity{
//            objectName: "x_axis"
//            components: [axis_mesh,x_axis_transform]
//        }
//        Entity{
//            objectName: "y_axis"
//            components: [axis_mesh,y_axis_transform]
//        }
//        Entity{
//            objectName:"z_axis"
//            components: [axis_mesh,z_axis_transform]
//        }
//        Transform{
//            property real z_fall:50
//            id:nextForce_transform
//            translation:Qt.vector3d(0,z_fall,0)
//        }
//        components: [nextForce_transform]
//    }




    property int zoom: 0
    property bool resize: false
    property int nbeams
    //property alias nbeams : nodeInstantiator.model

    property var scale_factors: []

    property int current_selection: 0


//    onNbeamsChanged: {
//        if(current_item && current_item.connected_beams){
//            scale_factors=[]
//            for (var i=0; i<nbeams+1; i++)
//                      scale_factors[i]=1
//            computeReaction();
//        }
//    }

//    property vector3d  reaction

//    //TODO: different supports
//    function computeReaction() {
//        //In 2D the rotation for flipped extremes is on Z, in 3d is on y. Swap y and z(double check)
//        reaction=Qt.vector3d(0,0,0)
//        if(scale_factors[0]!== 0)
//            for (var i=0; i<nbeams; i++){
//                var poseMatrix=current_item.connected_beams[i].poseMatrix;
//                var ab=current_item.connected_beams[i].extreme2.minus(current_item.connected_beams[i].extreme1).normalized();
//                var shearY;
//                var shearZ;
//                if(current_item.connected_beams[i].flip_extremes){
//                    console.log("Need a rotation on Z")
//                    shearY=poseMatrix.times(Qt.vector3d(0,current_item.connected_beams[i].shearYForce_extreme1,0))
//                    shearZ=poseMatrix.times(Qt.vector3d(0,0,current_item.connected_beams[i].shearZForce_extreme1))
//                    ab=ab.times(scale_factors[i+1]).times(current_item.connected_beams[i].axialForce_extreme1);
//                }
//                else{
//                    shearY=poseMatrix.times(Qt.vector3d(0,current_item.connected_beams[i].shearYForce_extreme1,0).times(-1))
//                    shearZ=poseMatrix.times(Qt.vector3d(0,0,current_item.connected_beams[i].shearZForce_extreme1).times(-1))
//                    ab=ab.times(scale_factors[i+1]).times(current_item.connected_beams[i].axialForce_extreme1).times(-1);
//                }
//                reaction=reaction.plus(ab.plus(shearY).plus(shearZ));
//            }
//        reaction=reaction.times(-1)
//        console.log(reaction)
//        console.log(current_item.reaction)

//    }



////    onResizeChanged: {
////        resizeAnimation.start();
////        resize=false;
////    }

////    QQ2.NumberAnimation {
////        id:resizeAnimation
////        target: deformingMeshTransformation;
////        property: "scale";
////        duration: 500;
////        to: 1
////        running: false
////        alwaysRunToEnd: true
////    }



//    Entity{
//        NodeInstantiator{
//            id:nodeInstantiator
//            delegate: Entity{
//                property var related_beam: current_item.connected_beams? current_item.connected_beams[index] : null
//                Entity{
//                    components:[
//                        CylinderMesh{
//                       //    property int zoom: 0
    //    property bool resize: false
    //    property alias nbeams : nodeInstantiator.model

    //    property var scale_factors: []

    //    property int current_selection: 0     id:cylinderMesh
//                            length: 20
//                        },
//                        Transform{
//                            id:cylinderTransform
//                            QQ2.Binding on translation {
//                                when: related_beam != null
//                                value:{
//                                    Qt.vector3d(0,-related_beam.axialForceType*cylinderMesh.length/2,0)
//                                }
//                            }
//                        }
//                    ]
//                }
//                Transform{
//                    QQ2.Binding on matrix {
//                        when: related_beam != null
//                        value: related_beam.poseMatrix
//                    }
//                    id:transformEntity
//                }
//                components: [transformEntity]
//            }
//        }
//    }
}


