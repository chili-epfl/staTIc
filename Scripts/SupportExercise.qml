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
    property url proposed_solution_url;

    property var loadsOnBeams: []
    function checkSolution(){
        if(default_script.settings.blink_displacement==2 || default_script.settings.blink_stress==2){
            score_text.text="Oh no!"
            score_text.color="red"
            crashSound.play();
        }else{
            score_text.text="Bravo!"
            score_text.color="green"
            clappingSound.play();
        }

    }
    DefaultScript{
        id:default_script
        onPageExit: parent.pageExit();
        stateLock: true
        onFirstInitChanged:
            if(!firstInit){
                for(var i=0;i<loadsOnBeams.length;i++){
                    var load=loadsOnBeams[i];
                    var beam=default_script.vmManager.getEntity3D(load.beamName);
                    if(beam){
                        default_script.vmManager.produceTPZForce(beam,
                                                             default_script.warehouse.get(
                                                                    load.warehouseIndex,"properties"));
                    }
                }
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
            AnchorChanges{
                target: next_button
                anchors.bottom: info_container.bottom
                anchors.right: info_container.right
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
            show_info_box:false
            show_beam_sphere: true
            show_stress_button:false
            show_displacement_button:false
            show_beam_axial_loads:false
            enable_scaffold:true
            show_legend:false
            show_filter_bar:false
            restoreEntryValues: false
            visible_loader:"CUSTOM"
        }
        PropertyChanges{
            target: info_container
            visible:false
        }
        PropertyChanges{
            target: next_button
            visible:true
        }
        AnchorChanges{
            target: next_button
            anchors.bottom: cable_exercise.bottom
            anchors.right: cable_exercise.right
        }
    },
        State {
            name: "SOLVED"
            PropertyChanges{
                target: info_container
                visible:true
            }
            StateChangeScript{
                script: {
                    default_script.stateLock=false
                    checkSolution()
                    default_script.infobox.custom_loader.source=""
                }
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
            AnchorChanges{
                target: next_button
                anchors.bottom: info_container.bottom
                anchors.right: info_container.right
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
         AnchorAnimation { duration: 500 }
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
            anchors.margins: 10
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
                anchors.left: problem_image.left
                anchors.leftMargin: 50
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
            }
        }

        Item{
            id:score_box
            anchors.fill: parent
            Text{
                id:score_text
                font.bold: true
                fontSizeMode: Text.Fit
                minimumPixelSize: 10; font.pixelSize: 50
                wrapMode: Text.WordWrap
                anchors.horizontalCenter: proposed_solution.horizontalCenter
                anchors.top:parent.top
                anchors.bottom: proposed_solution.top
                anchors.margins: 10
                lineHeight: 1.5
            }
            Image{
                id:proposed_solution
                anchors.centerIn: parent
                height: parent.height/2
                fillMode: Image.PreserveAspectFit
                source: proposed_solution_url
                Image{
                    width: parent.width/2
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: 50
                    source: "qrc:/scripts/Scripts/Story_1/boss_suggestion.png"
                }
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
                else if(cable_exercise.state=="SOLVING"){
                    cable_exercise.state="SOLVED"
                }
                else if(cable_exercise.state=="SOLVED"){
                    cable_exercise.state="EXPLORING"
                }
                else if(cable_exercise.state=="EXPLORING")
                    pageExit()
            }
        }

    }



}
