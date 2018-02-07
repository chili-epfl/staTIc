import QtQuick 2.0
import QtMultimedia 5.5

Item{
    visible: true
    signal exit();
    Camera {
        id: camera
        deviceId:QtMultimedia.availableCameras[0].deviceId
        viewfinder.resolution:"640x480"
        imageCapture {
            resolution:"640x480"
            onImageSaved: {
                currentImage = Qt.resolvedUrl("file://"+path)  // Show the preview in an Image
                console.log(path)
                exit();
            }
            onCaptureFailed: console.log(message)
        }
    }
    VideoOutput {
        source: camera
        anchors.fill: parent
        focus : true // to receive focus and capture key events when visible
        Image {
            id: shutter
            source: "qrc:/ui/UI/MaterialDesigner/assets/shutter-icon.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: 100
            height: width
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    camera.imageCapture.capture()
                    console.log("Click")
                }
            }
        }
    }
}
