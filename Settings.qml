import QtQuick 2.0

Item {

    property bool show_back_button:true
    property int blink_stress:0
    property int blink_displacement:0
    property bool show_stress: false
    property bool show_displacement:false
    property bool show_stress_button: false
    property bool show_AR_button: true
    property bool show_displacement_button:true
    property bool show_help_button:false
    property bool beam_is_selectable: true
    property bool joint_is_selectable: true
    property bool load_is_selectable: true
    property bool load_is_draggable: true
    property bool show_tangible_button:false
    property bool show_beam: true
    property bool show_beam_spring: true
    property bool show_beam_sphere: false
    property bool show_joint: true
    property bool show_beam_axial_loads: true
    property bool show_load: true
    property bool show_spatial_references: true
    property bool show_info_box:true
    property bool show_filter_bar: true
    property bool show_legend: true

    property bool enable_sounds:true

    property int exagerate_displacement_factor:1

    property string visible_loader:"DEFAULT"; //DEFAULT,CUSTOM

    property bool focus_on_joint:false


    /*Tools*/
    property bool enable_scaffold: false;
    property var beam_dragging_ownership:0;

    /*Focus View*/
    property vector3d focus_view_currentForce: Qt.vector3d(0,0,0)
    property vector3d focus_view_maxForce: Qt.vector3d(0.0001,0,0)

    property real focus_view_equilibrium_distance: focus_view_maxForce.length()>0.001 ? focus_view_currentForce.length()/focus_view_maxForce.length() : 0;

    property real focus_view_scaleFactor_focus:30/Math.max(Math.abs(focus_view_maxForce.x),Math.abs(focus_view_maxForce.y),Math.abs(focus_view_maxForce.z))


    function restore_defaults(){

        show_stress= false
        show_displacement=false
        show_stress_button= false
        show_displacement_button=true
        show_info_box=true

        beam_is_selectable= true
        joint_is_selectable= true
        load_is_selectable= true
        load_is_draggable=true

        show_beam= true
        show_beam_spring= true
        show_beam_sphere= false

        show_joint= true
        show_beam_axial_loads= true
        show_load= true
        show_spatial_references= true
        show_filter_bar= true
        show_legend= true

        enable_scaffold= false;
        show_AR_button= true
        visible_loader="DEFAULT";
        focus_on_joint=false;
        show_help_button=false
        show_tangible_button=false

    }

}
