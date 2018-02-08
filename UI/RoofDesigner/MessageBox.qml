import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2


Rectangle {
    id: message
    radius: 5
    border.width: 0
    anchors.centerIn: parent
    width: childrenRect.width+20
    height: childrenRect.height+40
    color: "#80000000"
    opacity: 0;

    property var statesName: ["error", "success", "info"];

    states: [
        State {
            name: "error"
            PropertyChanges {
                target: messageIcon
                text: "\uf071"
            }
        },
        State {
            name: "success"
            PropertyChanges {
                target: messageIcon
                text: "\uf00c"
            }
        },
        State {
            name: "info"
            PropertyChanges {
                target: messageIcon
                text: "\uf129"
            }
        }
    ]

    function changeState(stateName) {
        if(statesName.indexOf(stateName) == -1) {
            throw new Error("Bad state")
        }

        state = stateName;
    }

    function displayMessage(stateName, stateMessage, shouldHideAutomatically) {
        console.log("DisplayMessage(state: ", stateName, ", message: ", stateMessage, ")");
        changeState(stateName)
        messageContent.text = stateMessage;
        message.opacity = 1;
        if(shouldHideAutomatically) {
            hideTimer.restart();
        }
    }

    function displayErrorMessage(message) {
        displayMessage("error", message, true);
    }

    function displaySuccessMessage(message) {
        displayMessage("success", message, true);
    }

    function displayInfoMessage(message) {
        displayMessage("info", message, true);
    }

    function displayTutorialMessage(message) {
        displayMessage("info", "Start by ")
    }

    function hideMessage() {
        hideTimer.stop();
        message.opacity = 0;
    }

    Timer {
        interval: 10000
        running: false
        id: hideTimer
        onTriggered: {
            console.log("end of timer")
            hideMessage()
        }
    }

    Behavior on opacity {
        NumberAnimation { easing.type: Easing.InOutQuad; duration: 700 }
    }

    Component.onCompleted: {
        messageContentMouseArea.enabled = false
    }

    onOpacityChanged: {
        if(opacity < 1) {
            messageContentMouseArea.enabled = false;
        }
        else {
            messageContentMouseArea.enabled = true;
        }
    }

    ColumnLayout {
        y:20
        x:10
        spacing: 10
        Label {
            id: messageIcon
            text:""
            color: "#ffffff"
            font.pointSize: 53
            horizontalAlignment: Text.AlignHCenter
            font.family: "FontAwesome"
            Layout.fillWidth: true
            anchors.margins: 5
        }

        Label {
            id: messageContent
            color: "#ffffff"
            text: "Error message"
            anchors.margins: 5
            font.pointSize: 14

            Layout.fillWidth: true
        }
        MouseArea {
            id: messageContentMouseArea
            anchors.fill: parent
            onPressed: {
                hideTimer.restart()
            }
        }
    }
}
