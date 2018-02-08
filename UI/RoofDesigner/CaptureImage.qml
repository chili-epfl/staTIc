import QtQuick 2.7
import QtMultimedia 5.6
import QtQuick.Controls 2.1

Rectangle {
    id: capturePanel
    property bool isImageCaptured:false
    property url image_path
    Camera {
        id: camera
        focus.focusMode: Camera.FocusContinuous
        captureMode: Camera.CaptureStillImage
        imageCapture {
            onImageSaved: {
                isImageCaptured=true
                image_path="file://"+path
                photoPreview.source=image_path
            }
        }
    }

    VideoOutput {
        id: videoOutput
        source: camera
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onPressed: {
                camera.imageCapture.capture()
            }
        }
        autoOrientation: true
    }

    Image {
        id: photoPreview
        visible: isImageCaptured
        fillMode: Image.PreserveAspectFit
        anchors.fill: parent
    }

    Button {
        text: "Retake"
        anchors.leftMargin: 20
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        visible: isImageCaptured
        onClicked: {
            isImageCaptured = false
        }
    }
    Button {
        text: "Cancel"
        anchors.bottomMargin: 20
        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            sketchScreen.aux_loader.source=""
        }
    }

    Button {
            text: "Use it"
            anchors.bottomMargin: 20
            anchors.rightMargin: 20
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: isImageCaptured
            onClicked: {                
                sketchScreen.background_picture_url=image_path
                sketchScreen.aux_loader.source=""
            }
        }



    Rectangle {
        anchors.centerIn: parent
        color: "#20FFFFFF"
        border.color: "#80FFFFFF"
        radius: 5;
        visible: !isImageCaptured

        Label {
            text: "Touch the screen to take a picture"
            color: "#ffffff"
            anchors.centerIn: parent
        }
    }
}



