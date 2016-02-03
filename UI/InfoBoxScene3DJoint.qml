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

    property vector3d maxForce: Qt.vector3d(0.0001,0,0)

    property real equilibrium_distance: currentForce.length()/maxForce.length();

    property int scaleFactor: (Math.log(Math.max(Math.abs(maxForce.x),Math.abs(maxForce.y),Math.abs(maxForce.z)))/Math.LN10) >= 1 ?
                               Math.pow(10,Math.floor(Math.log(Math.max(Math.abs(maxForce.x),Math.abs(maxForce.y),Math.abs(maxForce.z)))/Math.LN10))
                              :
                              1
    onScaleFactorChanged: console.log(scaleFactor)
    onCurrentForceChanged: console.log(currentForce)

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

    /*Current Force Entity*/
    Entity{

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




        /*Solution 1*/
        Entity{
            enabled: false;
            PhongMaterial{
                id:color_centre_current_force
                property real h:0.33-0.23*equilibrium_distance
                ambient:Qt.hsla(h,1,0.32)
                diffuse:"grey"
                specular:"#000000"
                shininess:0

                QQ2.Behavior on h {
                    QQ2.NumberAnimation{duration: 1000}
                }

            }
            Mesh{
                id:half_sphere_mesh
                source:"qrc:/ui/UI/half_sphere.obj"
            }

            Entity{
                // x component
                id:x_component
                property real x_component_val: currentForce.x/scaleFactor
                QQ2.Behavior on x_component_val{
                    QQ2.NumberAnimation{
                        duration: 500
                    }
                }
                Entity{
                    Transform{
                        id:x_transform_pos
                        scale3D: x_component.x_component_val > 0 ?
                                     Qt.vector3d((2.5+x_component.x_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,x_transform_pos,color_centre_current_force]
                }
                Entity{
                    Transform{
                        id:x_transform_neg
                        rotation:fromAxisAndAngle(Qt.vector3d(0,1,0), 180)
                        scale3D: x_component.x_component_val < 0 ?
                                     Qt.vector3d((2.5-x_component.x_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,x_transform_neg,color_centre_current_force]
                }
            }

            Entity{
                // y component
                id:y_component
                property real y_component_val: currentForce.y/scaleFactor
                QQ2.Behavior on y_component_val{
                    QQ2.NumberAnimation{
                        duration: 500
                    }
                }
                Entity{
                    Transform{
                        id:y_transform_pos
                        rotation:fromAxisAndAngle(Qt.vector3d(0,0,1), 90)
                        scale3D: y_component.y_component_val > 0 ?
                                     Qt.vector3d((2.5+y_component.y_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,y_transform_pos,color_centre_current_force]
                }
                Entity{
                    Transform{
                        id:y_transform_neg
                        rotation:fromAxisAndAngle(Qt.vector3d(0,0,1), -90)
                        scale3D: y_component.y_component_val < 0 ?
                                     Qt.vector3d((2.5-y_component.y_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,y_transform_neg,color_centre_current_force]
                }
            }

            Entity{
                // z component
                id:z_component
                property real z_component_val: currentForce.z/scaleFactor
                QQ2.Behavior on z_component_val{
                    QQ2.NumberAnimation{
                        duration: 500
                    }
                }
                Entity{
                    Transform{
                        id:z_transform_pos
                        rotation:fromAxisAndAngle(Qt.vector3d(0,1,0), -90)
                        scale3D: z_component.z_component_val > 0 ?
                                     Qt.vector3d((2.5+z_component.z_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,z_transform_pos,color_centre_current_force]
                }
                Entity{
                    Transform{
                        id:z_transform_neg
                        rotation:fromAxisAndAngle(Qt.vector3d(0,1,0), 90)
                        scale3D: z_component.z_component_val < 0 ?
                                     Qt.vector3d((2.5-z_component.z_component_val),2.5,2.5):
                                     Qt.vector3d(2.5,2.5,2.5)
                    }
                    components: [half_sphere_mesh,z_transform_neg,color_centre_current_force]
                }

            }
        }
        /*End Solution 1*/

        /*Solution 2*/

        Entity{
           enabled: true;

           PhongAlphaMaterial{
               id:color_centre
               property real h: Math.max(1-equilibrium_distance,0.5)
               ambient:Qt.hsla(0.33,h,0.32)
               diffuse:"grey"
               specular:"#000000"
               shininess:0
               alpha: h
               QQ2.Behavior on h {
                   QQ2.NumberAnimation{duration: 1000}
               }

           }
           PhongMaterial{
               id:color_target
               property real h:0.33-0.23*equilibrium_distance
               ambient:Qt.hsla(h,1,0.32)
               diffuse:"grey"
               specular:"#000000"
               shininess:0
               QQ2.Behavior on h {
                   QQ2.NumberAnimation{duration: 1000}
               }

           }


           SphereMesh{
                id:sphere_mesh
                radius: 5 * axis_mesh.radius
           }

           Entity{
                  components: [color_centre,sphere_mesh]
           }
           Entity{

               Transform{
                    id: target_transform
                    translation: currentForce.times(1/scaleFactor)
                    QQ2.Behavior on translation{
                        QQ2.Vector3dAnimation{
                            duration: 500
                        }
                    }
               }
               components: [sphere_mesh,target_transform,color_target]

           }


        }


    }


}


