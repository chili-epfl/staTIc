import QtQuick 2.0

Item {

    property int blink_stress:0
    property int blink_displacement:0
    property bool show_stress: true
    property bool show_displacement:false
    property bool beam_is_selectable: true
    property bool joint_is_selectable: true
    property bool load_is_selectable: true
    property bool show_beam: true
    property bool show_beam_spring: true
    property bool show_joint: true
    property bool show_load: true
    property bool show_spatial_references: true
    property bool show_info_box:false
    /*Tools*/
    property bool enable_scaffold: true;
    property var beam_dragging_ownership:0;
    onBeam_dragging_ownershipChanged: console.log(beam_dragging_ownership)
}
