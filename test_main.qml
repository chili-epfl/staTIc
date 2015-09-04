import QtQuick 2.3 as QQ2
import QtQuick.Controls 1.3
import QtQuick.Window 2.0
import QtQuick.Scene3D 2.0
import Qt3D 2.0
import Qt3D.Renderer 2.0
ApplicationWindow {
    function pt2px(pt){return pt*0.3759*Screen.pixelDensity}
    id: applicationWindow
    visible: true
    //visibility: "Maximized"
    width: 1000
    height: 1000
    //DetailedViewJoint{}
    QQ2.Rectangle {
        color: "#8097CCF9";
        id: root;
        width: 500
        height: 500
        //anchors.fill: parent
            Scene3D {
                      width: 500
                      height: 500
                      focus: true
                      aspects: "input"
                      Entity {
                            id:sceneRoot
                            Camera {
                                id: camera
                                projectionType: CameraLens.OrthogonalProjection
                                nearPlane : 0.1
                                farPlane : 100000

                                top: 100
                                bottom: -100
                                left: -100
                                right: 100

                                position: Qt.vector3d( 0.0, 0.0, 0 )
                                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                                viewCenter: Qt.vector3d( 0.0, 0.0, -1.0 )
                            }
                            Configuration  {
                                controlledCamera: camera
                            }
                            components: [
                                    FrameGraph {
                                        activeFrameGraph:
                                            ForwardRenderer {
                                            clearColor: "red"
                                            camera:camera
                                            viewportRect:Qt.rect(0.25,0,1,1)
                                        }
                                    }
                                ]


                            CuboidMesh{
                                id:mesh
                                xExtent: 20
                                yExtent: 20
                                zExtent: 20
                            }
                            Transform{
                                id:transform
                                Translate{
                                    //dx:-20
                                    //dy:-20
                                    dz:-30
                                }
                            }
                            Entity {
                                components: [mesh,transform]
                            }
                        }
                    }

    }



}

