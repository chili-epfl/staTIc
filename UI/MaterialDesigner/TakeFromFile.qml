import QtQuick 2.0
import QtQuick.Dialogs 1.2

FileDialog {
    signal exit();
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.pictures
    nameFilters: [ "Image files (*.jpg *.png)"]
    onAccepted: {
        currentImage=fileDialog.fileUrl
        exit();
    }
    onRejected: {
        exit();
    }

    Component.onCompleted: visible = true

}

