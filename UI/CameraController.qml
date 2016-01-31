import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 10.0
    property real orbitSpeed: panMode? -180.0 : 90
    property real lookSpeed: panMode? 180.0 :-45
    property bool firstPersonMode: false
    property bool panMode: true

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
                if (panMode) {
                    if (root.firstPersonMode)
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                    else
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt)
                    root.camera.tiltAboutViewCenter(root.orbitSpeed * d.dy * dt)
                }
                else  {
                    if (root.firstPersonMode)
                        root.camera.pan(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                    else
                        root.camera.pan(root.lookSpeed * d.dx * dt)
                    root.camera.tilt(root.lookSpeed * d.dy * dt)
                }
            }
        }
    ] // components
}
