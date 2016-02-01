import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 100.0
    property real orbitSpeed: panMode? -90.0 : 90
    property real lookSpeed: panMode? 90.0 :-45
    property bool firstPersonMode: false
    property bool panMode: true
    property int zoom: 0
    property bool resize : false
    property int totalTranslationZ:0
    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)
        property real dx: 0
        property real dy: 0
    }

    MouseController {
        id: mouseSourceDevice
    }

    LogicalDevice {
        id: cameraControlDevice

        actions: [
            Action {
                name: "LMB"
                inputs: [
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        keys: [MouseController.Left]
                    }
                ]
            }
        ] // actions

        axes: [
            // Rotation
            Axis {
                name: "RX"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseController.X
                    }
                ]
            },
            Axis {
                name: "RY"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseController.Y
                    }
                ]
            }
        ] // axes
    }

    components: [
        AxisActionHandler {
            id: handler
            logicalDevice: cameraControlDevice
            onAxisValueChanged: {

                switch (name) {

                case "RX": {
                    d.dx = axisValue;
                    break;
                }

                case "RY": {
                    d.dy = axisValue;
                    break;
                }

                }
            }

        },
        LogicComponent {
            onFrameUpdate: {
                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.
                if(resize){
                    if(totalTranslationZ>0){
                        root.camera.translate(Qt.vector3d(0,0, -1*linearSpeed).times(dt))
                        totalTranslationZ=totalTranslationZ-1*linearSpeed*dt;
                    }
                    else if(totalTranslationZ<0){
                        root.camera.translate(Qt.vector3d(0,0, linearSpeed).times(dt))
                        totalTranslationZ=totalTranslationZ+linearSpeed*dt;
                    }
                    else{
                        resize=false;
                    }
                }
                else{
                    root.camera.translate(Qt.vector3d(0,0, zoom*linearSpeed).times(dt))
                    totalTranslationZ=totalTranslationZ+zoom*linearSpeed*dt;
                    if (panMode) {
                        if (root.firstPersonMode)
                            root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                        else
                            root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt)
                        root.camera.tiltAboutViewCenter(root.orbitSpeed * d.dy * dt)
                        //                    if (root.firstPersonMode)
                        //                        root.camera.pan(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                        //                    else
                        //                        root.camera.pan(root.lookSpeed * d.dx * dt)
                        //                    root.camera.tilt(root.lookSpeed * d.dy * dt)
                    }
                    else  {
                        var dx=0;
                        var dy=0;
                        if(d.dx > 0) dx=1;
                        else if(d.dx<0) dx=-1;
                        if(d.dy > 0) dy=1;
                        else if(d.dy<0) dy=-1;
                        root.camera.translate(Qt.vector3d(dx*linearSpeed, dy*linearSpeed, 0).times(dt))
                        //                    if (root.firstPersonMode)
                        //                        root.camera.pan(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                        //                    else
                        //                        root.camera.pan(root.lookSpeed * d.dx * dt)
                        //                    root.camera.tilt(root.lookSpeed * d.dy * dt)
                    }
                }
            }
        }
    ] // components
}
