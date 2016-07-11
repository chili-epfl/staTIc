import QtQuick 2.0

Rectangle{
    property alias text:suggestion_box.text
    onTextChanged: suggestion_box_container.state="Visible"
    id:suggestion_box_container
    state:"Hidden"
    states: [
        State {
            name: "Hidden"
            AnchorChanges {
                target: suggestion_box_container
                anchors.top:parent.bottom
            }
            PropertyChanges {
                target: suggestion_box_container
                opacity:0
            }
        },
        State {
            name: "Visible"
            AnchorChanges {
                target: suggestion_box_container
                anchors.bottom: parent.bottom
            }
            PropertyChanges {
                target: suggestion_box_container
                opacity:1
            }
            StateChangeScript{
                script: {suggention_box_timer.restart()}
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation { duration: 500 }
        NumberAnimation { property:"opacity" ; duration: 500 }
    }
    Timer{
        id:suggention_box_timer
        interval: 5000
        running: false
        onTriggered: suggestion_box_container.state="Hidden"
    }
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: 10
    width: Math.min(suggestion_box.implicitWidth+20,parent.width/3)
    height: Math.max(suggestion_box.implicitHeight+20,ghostMode_button.height)
    radius: 5
    border.width: 5
    border.color: "#B4E1E4"
    color: "#81c7e1"
    Text{
        id: suggestion_box
        anchors.fill: parent
        color: "white"
        text:""
        font.pointSize: 14
        fontSizeMode: Text.Fit;
        minimumPointSize: 10
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }


}

