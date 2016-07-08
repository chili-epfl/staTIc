import QtQuick 2.0

Item {

    property int blink_stress:0
    property int blink_displacement:0
    property bool show_stress: true
    property bool show_displacement:false
    property bool show_stress_button: true
    property bool show_displacement_button:true

    property bool beam_is_selectable: true
    property bool joint_is_selectable: true
    property bool load_is_selectable: true
    property bool show_beam: true
    property bool show_beam_spring: true
    property bool show_beam_sphere: false
    property bool show_joint: true
    property bool show_beam_axial_loads: true
    property bool show_load: true
    property bool show_spatial_references: true
    property bool show_info_box:false
    property bool show_filter_bar: true
    property bool show_legend: true

    /*Tools*/
    property bool enable_scaffold: true;
    property var beam_dragging_ownership:0;



    function restore_defaults(){

        show_stress= true
        show_displacement=false
        show_stress_button= true
        show_displacement_button=true

        beam_is_selectable= true
        joint_is_selectable= true
        load_is_selectable= true
        show_beam= true
        show_beam_spring= true
        show_beam_sphere= false
        show_joint= true
        show_beam_axial_loads= true
        show_load= true
        show_spatial_references= true
        show_info_box=false
        show_filter_bar= true
        show_legend= true

        enable_scaffold= true;



    }

}
