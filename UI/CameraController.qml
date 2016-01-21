import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2

Entity {
    id: root
    property Camera camera
    property real linearSpeed: 10.0
    property real orbitSpeed: -180.0
    property real lookSpeed: 180.0
    property bool firstPersonMode: true

    QtObject {
        id: d
        readonly property vector3d firstPersonUp: Qt.vector3d(0, 1, 0)
        property bool leftMouseButtonPressed: false
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

            onActionStarted: {

                switch (name) {

                case "LMB": {
                    d.leftMouseButtonPressed = true;
                    break;
                }

                }

            }

            onActionFinished: {

                switch (name) {

                case "LMB": {
                    d.leftMouseButtonPressed = false;
                    break;
                }

                }
            }
        },
        LogicComponent {
            onFrameUpdate: {
                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.
                if (d.leftMouseButtonPressed) {
                    if (root.firstPersonMode)
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt, d.firstPersonUp)
                    else
                        root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt)
                    root.camera.tiltAboutViewCenter(root.orbitSpeed * d.dy * dt)
                }
            }
        }
    ] // components
}
