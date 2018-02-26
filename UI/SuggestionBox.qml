import QtQuick 2.0
import QtQuick.Controls 2.3
Rectangle{

    function show_message(text){
        suggestion_box.text=text;
        suggestion_box_container.state="Visible"
    }
    function show_alert_message(text){
        suggestion_box.text=text;
        suggestion_box_container.state="Alert"
    }
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
                anchors.verticalCenter: parent.verticalCenter
            }
            PropertyChanges {
                target: suggestion_box_container
                opacity:1
            }
        },
        State{
            name:"Alert"
            extend: "Visible"
            StateChangeScript{
                script: timer.start()
            }
            PropertyChanges {
                target: suggestion_box_container
                color:"#F44336"
                border.color:"#EF5350"
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation { duration: 500 }
        NumberAnimation { property:"opacity" ; duration: 500 }
    }
    Timer{
        id:timer
        interval: 5000
        running: false
        repeat: false
        onTriggered: suggestion_box_container.state="Hidden"
    }
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: 10
    width: parent.width/3+20
    height: suggestion_box.implicitHeight+20
    radius: 5
    border.width: 5
    border.color: "#B4E1E4"
    color: "#81c7e1"
    Text{
        id: suggestion_box
        anchors.centerIn: parent
        color: "white"
        text:""
        width: parent.width-20
        font.pointSize: 14
        fontSizeMode: Text.Fit;
        minimumPointSize: 12
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignJustify
        verticalAlignment: Text.AlignVCenter
    }
    RoundButton{
        text:"\u274C"
        onClicked:suggestion_box_container.state="Hidden"
        anchors.right: parent.left
        anchors.bottom: parent.top
    }
}
