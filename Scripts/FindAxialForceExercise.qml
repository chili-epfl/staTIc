import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtMultimedia 5.5

Item {
    id:exercise
    anchors.fill: parent;
    signal pageExit();

    property alias structureUrl:default_script.structureUrl;
    property alias structure3DAsset:default_script.structure3DAsset;

    property string problem_description_text:"";
    property url problem_image_url;
    property url proposed_solution_url;

    property var question_beams;
    property var loadsOnBeams;

    property var hidden_beams;

    property var elements_to_restore:[];


    ListModel {
        id:question_beams_model
    }

    DefaultScript{
        id:default_script
        onPageExit: parent.pageExit();
        stateLock: true
        onFirstInitChanged:
            if(!firstInit){
                default_script.infobox.custom_loader.source="qrc:/ui/UI/InfoBoxFindAxialForce.qml"
                for(var i=0;i<question_beams.length;i++){
                    var beam_name=question_beams[i];
                    var beam=vmManager.getEntity3D(beam_name);
                    if(beam){
                        question_beams_model.append({"beam":beam, "answer":0,"correctness":0})
                    }
                    else{
                       console.log("Problem with getting the beam:",beam_name)
                    }
                    var e1=vmManager.getEntity3D(beam_name[0])
                    var e2=vmManager.getEntity3D(beam_name[1])
                    if(e1){
                        for(var j=0;j<e1.connected_beams.length;j++){
                            var b=e1.connected_beams[j]
                            if(b.extreme2_name==e2.objectName){
                                b.non_default_visibility=false;
                                elements_to_restore.push(b);
                            }
                        }}
                    if(e2){
                        for(j=0;j<e2.connected_beams.length;j++){
                            var b=e2.connected_beams[j]
                            if(b.extreme2_name==e1.objectName){
                                b.non_default_visibility=false;
                                elements_to_restore.push(b);
                            }
                        }}
                }

                for( i=0;i<hidden_beams.length;i++){
                    beam_name=hidden_beams[i];
                    beam=vmManager.getEntity3D(beam_name);
                    if(!beam)
                        console.log("Problem with getting the beam:",beam_name)

                    e1=vmManager.getEntity3D(beam_name[0])
                    e2=vmManager.getEntity3D(beam_name[1])
                    if(e1){
                        for(j=0;j<e1.connected_beams.length;j++){
                            b=e1.connected_beams[j]
                            if(b.extreme2_name==e2.objectName){
                                b.non_default_visibility=false;
                                elements_to_restore.push(b);
                            }
                        }}
                    if(e2){
                        for(j=0;j<e2.connected_beams.length;j++){
                            b=e2.connected_beams[j]
                            if(b.extreme2_name==e1.objectName){
                                b.non_default_visibility=false;
                                elements_to_restore.push(b);
                            }
                        }}
                }


                for(i=0;i<loadsOnBeams.length;i++){
                    var load=loadsOnBeams[i];
                    beam=default_script.vmManager.getEntity3D(load.beamName);
                    if(beam){
                        default_script.vmManager.produceTPZForce(beam,
                                                             default_script.warehouse.get(
                                                                    load.warehouseIndex,"properties"));
                    }
                }
            }
    }

    Binding {
        when: default_script.infobox.custom_loader.status == Loader.Ready
        target: default_script.infobox.custom_loader.item
        property: "question_beams"
        value:question_beams_model
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
            beam_is_selectable: false
            joint_is_selectable: false
            load_is_selectable: false
            load_is_draggable:false
            show_beam: true
            show_beam_spring: false
            show_joint: true
            show_load: false
            show_spatial_references: false
            show_info_box:true
            show_beam_sphere: true
            show_stress_button:false
            show_displacement_button:false
            show_beam_axial_loads:true
            enable_scaffold:false
            show_legend:true
            show_filter_bar:false
            visible_loader:"CUSTOM"
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
        StateChangeScript{
            script: logger.log("FindAxialLoad_Exercise_Solving")
        }
    },
        State {
            name: "SOLVED"
            PropertyChanges{
                target: info_container
                visible:false
                restoreEntryValues: false
            }
            StateChangeScript{
                script: {
                    for(var i=0;i<elements_to_restore.length;i++)
                        elements_to_restore[i].non_default_visibility=true;
                }
            }
            PropertyChanges{
                target: story_box
                visible:false
                restoreEntryValues: false
            }
            PropertyChanges{
                target: score_box
                visible:false
                restoreEntryValues: false
            }
            PropertyChanges{
                target: next_button
                visible:true
                restoreEntryValues: false
            }
            AnchorChanges{
                target: next_button
                anchors.bottom: exercise.bottom
                anchors.right: exercise.right
            }

        },
        State {
            name: "EXPLORING"
            StateChangeScript{
                script: {
                    default_script.stateLock=false
                    default_script.infobox.custom_loader.source=""
                }
            }
            PropertyChanges{
                target: info_container
                visible:false
            }

            StateChangeScript{
                script: logger.log("FindAxialLoad_Exercise_Exploring")
            }
            AnchorChanges{
                target: next_button
                anchors.bottom: exercise.bottom
                anchors.right: exercise.right
            }
        }

    ]

    transitions: Transition {
         AnchorAnimation { duration: 1000 }
     }

    Connections{
        target: default_script.infobox.custom_loader.item
        ignoreUnknownSignals: true
        onExerciseCompleteChanged: {
            correct_text.text=default_script.infobox.custom_loader.item.correct;
            wrong_text.text=default_script.infobox.custom_loader.item.wrong;
            missing_text.text=default_script.infobox.custom_loader.item.missing;
            if(default_script.infobox.custom_loader.item.wrong>0)
                crashSound.play()
            else
                clappingSound.play()
            exercise.state="SOLVED"
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
        width: exercise.state=="PRESENTING" ? 0.7*parent.width :0.5*parent.width
        height: exercise.state=="PRESENTING" ? 0.7*parent.height :0.5*parent.height
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
            Rectangle{
                anchors.horizontalCenter: parent.horizontalCenter
                width: 100
                height: 100
                anchors.bottom: parent.bottom
                Image {
                    source: "qrc:/ui/UI/Eye_tracking_tags/011.png"
                    anchors.centerIn: parent
                    width: parent.width*0.75
                    height: parent.height*0.75
                }
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
                if(exercise.state=="PRESENTING")
                    exercise.state="SOLVING"
                else if(exercise.state=="SOLVED"){
                    exercise.state="EXPLORING"
                }
                else if(exercise.state=="EXPLORING")
                    pageExit()
            }
        }

    }



}
