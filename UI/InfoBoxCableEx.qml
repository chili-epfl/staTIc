import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtMultimedia 5.5
import "qrc:/ui/UI"



Rectangle {
    id:root
    property bool exerciseComplete:false;
    SoundEffect {
        id: crashSound
        source: "qrc:/soundeffects/AR/SoundEffects/crash_wood_2.wav"
    }
    SoundEffect {
        id: clappingSound
        source: "qrc:/soundeffects/AR/SoundEffects/clapping.wav"
    }
    function checkSolution(){
        var valid=true;
        for(var i=0; i<selected_beams.count;i++){
                if(selected_beams.get(i).beam.axialForceType==-1){
                    valid=false
                    crashSound.play();
                    break;
                }
          }
        if(valid)
            clappingSound.play()
        exerciseComplete=true;
    }
    property var current_item
    color: "transparent"
    onCurrent_itemChanged: {
        if(current_item != 0 && current_item.type=="beam"){
            var present=false;
            for(var i=0; i<selected_beams.count;i++){
                    if(current_item==selected_beams.get(i).beam){
                        present=true;
                        break;
                    }
              }
            if(!present){
                selected_beams.append({beam:current_item});
            }
        }
    }
    ListModel{
        id:selected_beams
    }

    ColumnLayout{
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        Rectangle{
            color:"transparent"
            border.color: "#F0F0F0"
            border.width: 5
            radius:1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.10*root.height
            Rectangle{
                color:"#2f3439"
                radius: 1
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    wrapMode: Text.WordWrap
                    anchors.fill: parent
                    anchors.margins: 10
                    id: title
                    text: qsTr("Select the beams which can be replaced by a tie-beam")
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10; font.pixelSize: 32
                    color: "white"
                }

            }
        }
        Rectangle{
            color:"transparent"
            border.color: "#F0F0F0"
            border.width: 5
            radius:1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.50*root.height
            ListView{
                id:list_beams
                anchors.fill: parent
                model: selected_beams
                spacing:10
                anchors.margins: 10
                clip:true
                delegate: Rectangle{
                    color:"#2f3439"
                    width: root.width-20
                    height: 0.10*root.height
                    anchors.margins: 10
                    RowLayout{
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        Image {
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.height
                            fillMode: Image.PreserveAspectFit
                            source: remove_button.pressed ? "qrc:/icons/Icons/delete_pressed.png":"qrc:/icons/Icons/delete.png"
                            MouseArea{
                                id:remove_button
                                anchors.fill: parent
                                onClicked:{
                                    selected_beams.remove(index)
                                }
                            }
                        }
                        Image {
                            Layout.preferredWidth: 0.50*root.width
                            Layout.preferredHeight: parent.height
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/images/Images/wood_beam2.png"
                        }
                        Item{
                            Layout.fillWidth: true
                            Layout.preferredHeight: parent.height
                            Text{
                                anchors.centerIn: parent
                                color:"#F0F0F0"
                                id:beam_label
                                text:beam.objectName
                                fontSizeMode: Text.Fit
                                minimumPixelSize: 10; font.pixelSize: 32
                            }
                        }
                    }
                }
            }
        }
        Item{
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.10*root.height
            Rectangle{
                color:"transparent"
                border.color: "#F0F0F0"
                border.width: 5
                radius:1
                width: parent.height
                height: parent.height
                anchors.centerIn: parent
                Rectangle{
                    anchors.fill: parent
                    anchors.margins: 10
                    color: "#2f3439"
                Image{
                    height: parent.height
                    width: height
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    anchors.margins: 10
                    source: verify_button.pressed ? "qrc:/icons/Icons/apply_pressed.png":"qrc:/icons/Icons/apply.png"
                    MouseArea{
                        id:verify_button
                        anchors.fill: parent
                        onClicked: {
                              checkSolution();
                        }
                    }
                }
                }
            }
        }

    }
}

