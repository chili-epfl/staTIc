import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import "qrc:/ui/UI"

Rectangle {
    id:root
    property bool exerciseComplete:false;
    property int correct:0
    property int missing:0
    property int wrong: 0

    property ListModel question_beams;

    function checkSolution(){
        var logger_map={}
        for(var i=0; i<question_beams.count;i++){
            logger_map[question_beams.get(i).beam.objectName]=question_beams.get(i).answer;
            if(question_beams.get(i).answer==0)
                missing++;
            else if(question_beams.get(i).beam.axialForceType!=question_beams.get(i).answer){
                wrong++
            }
            else {
                correct++
            }
        }
        logger_map["Correct"]=correct;
        logger_map["Missing"]=missing
        logger_map["Wrong"]=wrong;
        logger.log("FindAxialForce_check_solution",logger_map)
        exerciseComplete=true;
    }

    color: "transparent"


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
                    text: qsTr("Identify the axial stress type in each element")
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
                model: question_beams
                spacing:10
                anchors.margins: 10
                clip:true
                delegate: Rectangle{
                    color:"#2f3439"
                    width: root.width-20
                    height: (tension_check_box.implicitHeight+20)*2
                    anchors.margins: 10
                    RowLayout{
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        Item{
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width/2
                            Text{
                                anchors.centerIn: parent
                                color:"#F0F0F0"
                                id:beam_label
                                text:beam.objectName
                                fontSizeMode: Text.Fit
                                minimumPixelSize: 10; font.pixelSize: 32
                            }
                        }
                        ColumnLayout{
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width/2
                            CheckBox{
                                id:tension_check_box
                                onCheckedChanged: if(checked){
                                                      question_beams.get(index).answer=1
                                                      if(compression_check_box.checked)
                                                          compression_check_box.checked=false;
                                                  }else{
                                                      if(!compression_check_box.checked)
                                                          question_beams.get(index).answer=0
                                                  }

                                Text{
                                    text: qsTr("Tension")
                                    color:"#F0F0F0"
                                    anchors.left: parent.right
                                    height: parent.height
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                            CheckBox{
                                id:compression_check_box
                                onCheckedChanged: if(checked){
                                                      question_beams.get(index).answer=-1
                                                      if(tension_check_box.checked)
                                                          tension_check_box.checked=false;
                                                  }
                                                  else{
                                                      if(!tension_check_box.checked)
                                                          question_beams.get(index).answer=0
                                                  }
                                Text{
                                    text: qsTr("Compression")
                                    color:"#F0F0F0"
                                    anchors.left: parent.right
                                    height: parent.height
                                    verticalAlignment: Text.AlignVCenter
                                }
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


