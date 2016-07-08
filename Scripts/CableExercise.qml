import QtQuick 2.0

Item {
    id:cable_exercise
    anchors.fill: parent;
    signal pageExit();


    property alias structureUrl:default_script.structureUrl;
    property alias structure3DAsset:default_script.structure3DAsset;
    property alias structureTagConfig:default_script.structureTagConfig;

    DefaultScript{
        id:default_script
        onPageExit: parent.pageExit();
        stateLock: true
        onFirstInitChanged:
            if(!firstInit){
                default_script.infobox.loader.source="qrc:/ui/UI/InfoBoxCableEx.qml"
                cable_exercise.state="SOLVING"
            }
    }

    states:[
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
    },
        State {
            name: "SOLVED"
            StateChangeScript{
                script: default_script.stateLock=false
            }
        }
    ]

    Connections{
        target: default_script.infobox.loader.item
        ignoreUnknownSignals: true
        onExerciseCompleteChanged: {cable_exercise.state="SOLVED"}
    }

}
