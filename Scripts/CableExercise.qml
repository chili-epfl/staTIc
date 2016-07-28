import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtMultimedia 5.5

Item {
    id:cable_exercise
    anchors.fill: parent;
    signal pageExit();

    property alias structureUrl:default_script.structureUrl;
    property alias structure3DAsset:default_script.structure3DAsset;
    property alias structureTagConfig:default_script.structureTagConfig;

    property string problem_description_text;
    property url problem_image_url;

    DefaultScript{
        id:default_script
        onPageExit: parent.pageExit();
        stateLock: true
        onFirstInitChanged:
            if(!firstInit){
                default_script.infobox.loader.source="qrc:/ui/UI/InfoBoxCableEx.qml"
            }
    }
    state: "PRESENTING"
    states:[
        State {
            name: "PRESENTING"
            extend: "SOLVING"
            PropertyChanges{
                target: story_box
                visible:true
            }
            PropertyChanges{
                target: score_box
                visible:false
            }
            PropertyChanges{
                target: info_container
                visible:true

            }
            PropertyChanges{
                target: next_button
                visible:true
            }
            PropertyChanges{
                target: default_script.settings
                show_back_button:false
                restoreEntryValues: false
            }
        },
        State {
        name: "SOLVING"
        PropertyChanges {
            target: default_script.settings
            show_stress: false
            show_displacement:false
            beam_is_selectable: true
            joint_is_selectable: false
            load_is_selectable: false
            show_beam: true
            show_beam_spring: false
            show_joint: true
            show_load: false
            show_spatial_references: false
            show_info_box:true
            show_beam_sphere: true
            show_stress_button:false
            show_displacement_button:false
            show_beam_axial_loads:false
            enable_scaffold:false
            show_legend:false
            show_filter_bar:false
            restoreEntryValues: false
        }
        PropertyChanges {
            target: default_script.infobox
            lateral_visibility: "Visible"
        }
        PropertyChanges{
            target: info_container
            visible:false
        }
        PropertyChanges{
            target: next_button
            visible:false
        }
    },
        State {
            name: "SOLVED"
            PropertyChanges{
                target: info_container
                visible:true
            }
            StateChangeScript{
                script: default_script.stateLock=false
            }
            PropertyChanges{
                target: story_box
                visible:false
                restoreEntryValues: false
            }
            PropertyChanges{
                target: score_box
                visible:true
                restoreEntryValues: false
            }
            PropertyChanges{
                target: next_button
                visible:true
                restoreEntryValues: false
            }
        },
        State {
            name: "EXPLORING"
            PropertyChanges{
                target: info_container
                visible:false
            }
            AnchorChanges{
                target: next_button
                anchors.bottom: cable_exercise.bottom
                anchors.right: cable_exercise.right
            }
        }

    ]
    transitions: Transition {
         AnchorAnimation { duration: 1000 }
     }

    Connections{
        target: default_script.infobox.loader.item
        ignoreUnknownSignals: true
        onExerciseCompleteChanged: {
            correct_text.text=default_script.infobox.loader.item.correct;
            wrong_text.text=default_script.infobox.loader.item.wrong;
            missing_text.text=default_script.infobox.loader.item.missing;
            if(default_script.infobox.loader.item.wrong>0)
                crashSound.play()
            else
                clappingSound.play()
            cable_exercise.state="SOLVED"
        }
    }

    SoundEffect {
        id: crashSound
        source: "qrc:/soundeffects/AR/SoundEffects/crash_wood_2.wav"

    }

    SoundEffect {
        id: clappingSound
        source: "qrc:/soundeffects/AR/SoundEffects/clapping.wav"
    }


    Rectangle{
        id:info_container
        width: cable_exercise.state=="PRESENTING" ? 0.7*parent.width :0.5*parent.width
        height: cable_exercise.state=="PRESENTING" ? 0.7*parent.height :0.5*parent.height
        anchors.centerIn: parent
        color: "#2f3439"
        radius: 5
        border.width: 10
        border.color: "#F0F0F0"
        Item {
            id:story_box
            anchors.fill: parent

            Image {
                id: problem_image
                source: problem_image_url
                anchors.margins: 10
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.horizontalCenter
            }
            Image {
                id: master_icon
                source: "qrc:/images/Images/boss.png"
                width: 100
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: problem_image.bottom
                anchors.horizontalCenter: problem_image.horizontalCenter
            }
            Text{
                fontSizeMode: Text.Fit
                minimumPixelSize: 10; font.pixelSize: 32
                wrapMode: Text.WordWrap
                id:problem_description
                text: problem_description_text
                color: "white"
                anchors.margins: 10
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.left: parent.horizontalCenter
                textFormat: Text.RichText
                lineHeight: 1.5

            }
        }
        Item{
            id:score_box
            anchors.fill: parent
            GridLayout{
                width: 0.9*parent.width
                height: 0.9*parent.height
                anchors.centerIn: parent
                columns: 2
                rows: 3
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text{
                        text: "Correct:"
                        font.bold: true
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        wrapMode: Text.WordWrap
                        color: "Green"
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text{
                        id:correct_text
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        wrapMode: Text.WordWrap
                        color: "#F0F0F0"
                        anchors.verticalCenter: parent.verticalCenter
                    }}
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text{
                        text:"Missing:"
                        font.bold: true
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        wrapMode: Text.WordWrap
                        color: "yellow"
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                    }}
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text{
                        id:missing_text
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        wrapMode: Text.WordWrap
                        color: "#F0F0F0"
                        anchors.verticalCenter: parent.verticalCenter
                    }}
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                Text{
                    text:"Wrong:"
                    font.bold: true
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10; font.pixelSize: 50
                    wrapMode: Text.WordWrap
                    color: "red"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }}
                Item{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text{
                        id:wrong_text
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10; font.pixelSize: 50
                        wrapMode: Text.WordWrap
                        color: "#F0F0F0"
                        anchors.verticalCenter: parent.verticalCenter
                    }}
            }
        }
    }
    Image{
        id:next_button
        source:next_button_area.pressed ? "qrc:/icons/Icons/next_pressed.png" : "qrc:/icons/Icons/next.png"
        width: 100
        fillMode: Image.PreserveAspectFit
        anchors.margins: 10
        anchors.bottom: info_container.bottom
        anchors.right: info_container.right
        MouseArea{
            id:next_button_area
            anchors.fill: parent
            onClicked: {
                if(cable_exercise.state=="PRESENTING")
                    cable_exercise.state="SOLVING"
                else if(cable_exercise.state=="SOLVED"){
                    cable_exercise.state="EXPLORING"
                }
                else if(cable_exercise.state=="EXPLORING")
                    pageExit()
            }
        }

    }



}
