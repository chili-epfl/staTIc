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
    property string init_state: "SOLVING"
    property url problem_image_url;
    property url proposed_solution_url;

    property int exerciseID;

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
                if(exercise.init_state== "SOLVING"){
                    default_script.infobox.width=default_script.infobox.parent.width/6
                    default_script.infobox.custom_loader.source="qrc:/ui/UI/InfoBoxFindAxialForce_v2.qml"
                    for(var i=0;i<question_beams.length;i++){
                        var beam_name=question_beams[i];
                        var beam=default_script.sceneRoot.findEntityByName(beam_name);

                        if(beam){
                            question_beams_model.append({"beam":beam, "answer":0,"correctness":0})
                            beam.highlighted=true
                            beam.non_default_visibility=false;
                        }
                        else{
                            console.log("Problem with getting the beam:",beam_name)
                        }

                        var e1=default_script.sceneRoot.findEntityByName(beam_name[0])
                        var e2=default_script.sceneRoot.findEntityByName(beam_name[1])
                        if(e1){
                            for(var j=0;j<e1.connected_beams_instatiator.count;j++){
                                var b=e1.connected_beams_instatiator.objectAt(j)
                                if(b.extreme2_name==e2.objectName){
                                    b.non_default_visibility=false;
                                    elements_to_restore.push(b);
                                }
                            }
                        }
                        if(e2){
                            for(j=0;j<e2.connected_beams_instatiator.count;j++){
                                var b=e2.connected_beams_instatiator.objectAt(j)
                                if(b.extreme2_name==e1.objectName){
                                    b.non_default_visibility=false;
                                    elements_to_restore.push(b);
                                }
                            }
                        }
                    }

                    for( i=0;i<hidden_beams.length;i++){
                        beam=default_script.sceneRoot.findEntityByName(hidden_beams[i]);
                        beam_name=hidden_beams[i];
                        if(!beam)
                            console.log("Problem with getting the beam:",beam_name)
                        beam.non_default_visibility=false;

                        e1=default_script.sceneRoot.findEntityByName(beam_name[0])
                        e2=default_script.sceneRoot.findEntityByName(beam_name[1])
                        if(e1){
                            for(j=0;j<e1.connected_beams_instatiator.count;j++){
                                b=e1.connected_beams_instatiator.objectAt(j)
                                if(b.extreme2_name==e2.objectName){
                                    b.non_default_visibility=false;
                                    elements_to_restore.push(b);
                                }
                            }}
                        if(e2){
                            for(j=0;j<e2.connected_beams_instatiator.count;j++){
                                b=e2.connected_beams_instatiator.objectAt(j)
                                if(b.extreme2_name==e1.objectName){
                                    b.non_default_visibility=false;
                                    elements_to_restore.push(b);
                                }
                            }}
                    }

                }
                for(i=0;i<loadsOnBeams.length;i++){
                    var load=loadsOnBeams[i];
                    beam=default_script.sceneRoot.findEntityByName(load.beamName);
                    if(beam){
                        default_script.staticsModule.createUDLoad(beam.backend_entity,{"parent_entity":beam,"warehouse_index":load.warehouseIndex});
                    }
                }

                next_button.visible=true
                exercise.state= exercise.init_state

            }
    }

    Binding {
        when: default_script.infobox.custom_loader.status == Loader.Ready
        target: default_script.infobox.custom_loader.item
        property: "question_beams"
        value:question_beams_model
    }

    states:[
        State {
        name: "SOLVING"
        PropertyChanges {
            target: default_script.settings
            show_back_button:false
            show_stress: false
            show_displacement:false
            beam_is_selectable: false
            joint_is_selectable: false
            load_is_selectable: false
            load_is_draggable:false
            show_beam: true
            show_beam_spring: condition=="Springs" ? true:false
            show_joint: true
            show_load: false
            show_spatial_references: false
            show_info_box:true
            show_beam_sphere: condition=="Springs" ? false:true
            show_stress_button:false
            show_displacement_button:false
            show_beam_axial_loads:condition=="Springs" ? false:true
            enable_scaffold:false
            show_legend: true
            show_filter_bar:false
            visible_loader:"CUSTOM"
            restoreEntryValues: false
        }
        PropertyChanges {
            target: default_script.infobox
            lateral_visibility: "Visible"
        }
        PropertyChanges{
            target: next_button
            visible:true
        }
        StateChangeScript{
            script: {
                logger.log("FindAxialLoad_Exercise_Solving")
            }
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
            StateChangeScript{
                script: logger.log("FindAxialLoad_Exercise_Exploring")
            }
        }

    ]



    Image{
        id:next_button
        visible: false
        source:next_button_area.pressed ? "qrc:/icons/Icons/next_pressed.png" : "qrc:/icons/Icons/next.png"
        width: 100
        fillMode: Image.PreserveAspectFit
        anchors.margins: 10
        anchors.bottom: exercise.bottom
        anchors.right: exercise.right
        MouseArea{
            id:next_button_area
            anchors.fill: parent
            property bool firstClick:true;
            onClicked: {
                if(!firstClick)
                    default_script.pageExit()
                else if(firstClick){
                    default_script.suggestion_box.show_message("Click again to exit")
                    firstClick=false
                    timer_reset_firstClick.start()
                }
            }
            Timer{
                id:timer_reset_firstClick
                interval: 5000
                running: false
                onTriggered: next_button_area.firstClick=true
            }
            Timer{
                interval: 2000
                id:latency
                running: false
                onTriggered: next_button_area.enabled=true;
            }
        }

    }


}
