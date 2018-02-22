import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
Rectangle {
    id:_root
    color:"#2f3439"
    radius:5
    border.color: "white"
    height: (x_text.font.pixelSize+10)*10
    property alias global_orientation: orientation_slider.value
    property bool show_xy: false
    property int current_id:-1;
    signal update_Model(int origin_id)
    onCurrent_idChanged: {
        updateView()
    }

    function updateView(){
        origin_checkbox_connections.target=null
        var orientation=grid_repeater.itemAt(current_id).orientation;
        if(orientation==0)
            orientation_combobox.currentIndex=0;
        else if(orientation==60)
            orientation_combobox.currentIndex= 1;
        else if(orientation==120)
            orientation_combobox.currentIndex= 2;
        else if(orientation==180)
            orientation_combobox.currentIndex= 3;
        else if(orientation==-60)
            orientation_combobox.currentIndex= 4;
        else if(orientation==-120)
            orientation_combobox.currentIndex= 5;
        origin_checkbox.checked=grid_repeater.itemAt(current_id).origin
        origin_checkbox_connections.target=origin_checkbox
    }

    Drag.active: dragArea.drag.active
    x:_root.parent.width-(_root.width+10)
    y:_root.parent.height/2-_root.height/2

    Rectangle{
        id:hideButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        state: "Visible"
        states:[
            State {
                name: "Hidden"
                PropertyChanges {
                    target: hideLabel
                    text: "Show"
                }
                AnchorChanges {
                    target: _root
                    anchors.verticalCenter: _root.parent.verticalCenter
                    anchors.left: _root.parent.right
                }
                PropertyChanges{
                    target:_root
                    anchors.leftMargin: -hideLabel.height*2
                }

            },
            State {
                name: "Visible"
                PropertyChanges {
                    target: hideLabel
                    text: "Hide"
                }

            }
        ]
        transitions: Transition {
            AnchorAnimation { duration: 500 }
        }
        color:"#2f3439"
        height:  Math.max(parent.width * 0.25,hideLabel.width*1.2)
        width: hideLabel.font.pixelSize*1.10
        Label{
            id:hideLabel
            text: "Hide"
            color: "#F8F8F8"
            rotation: -90
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            onPressed: hideButton.state==="Visible" ? hideButton.state="Hidden": hideButton.state="Visible"
        }

    }

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 10
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: x_text.font.pixelSize+10
            Image{
                height: parent.height
                fillMode: Image.PreserveAspectFit
                source: "qrc:/ui/UI/BoardDesigner/Cursor-Move-512.png"
                mipmap: true
                MouseArea{
                    id:dragArea
                    anchors.fill: parent
                    drag.target: _root
                }
                Label{
                    visible:current_id==-1
                    anchors.left: parent.right
                    text:"Click on a marker"
                    color:"white"
                }
            }
        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: x_text.font.pixelSize+10
            Label{
                id:x_text
                visible: current_id!=-1
                anchors.rightMargin: 10
                text: "X:"+Math.round(grid_repeater.itemAt(current_id).world_x*100)/100
                anchors.right: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
            Label{
                visible: current_id!=-1
                text:"Y:"+Math.round(grid_repeater.itemAt(current_id).world_y*100)/100
                anchors.left: parent.horizontalCenter
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(x_text.font.pixelSize+10,origin_checkbox.height+10)
            Label{
                visible: current_id!=-1
                text: "Origin:"
                anchors.right: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "white"

            }
            CheckBox{
                id:origin_checkbox
                visible: current_id!=-1
                checked: false
                anchors.left: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                enabled: !checked
                Connections{
                    id:origin_checkbox_connections
                    target:null
                    onCheckedChanged: {
                        if(origin_checkbox.checked){
                            grid_repeater.itemAt(current_id).origin=true;
                            _root.update_Model(current_id);
                        }
                    }
                }


            }
        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(x_text.font.pixelSize+10,orientation_combobox.height+10)
            Label{
                visible: current_id!=-1
                text: "Orientation:"
                anchors.right: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
            ComboBox{
                visible: current_id!=-1
                id:orientation_combobox
                width: parent.width*0.5-10
                model: [0,60,120,180,-60,-120]
                anchors.left: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                onCurrentIndexChanged: grid_repeater.itemAt(current_id).orientation=Number(currentText)
            }
        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: x_text.font.pixelSize+20+orientation_slider.height
            Label{
                id:glob_orientation_label
                text: "Global Orientation: "+orientation_slider.value
                color: "white"
            }
            Slider{
                id:orientation_slider
                width: parent.width-40
                anchors.top: glob_orientation_label.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                stepSize: 60
                minimumValue: -120
                maximumValue: 180
                value: 0
                tickmarksEnabled: true
                Component.onCompleted: value=0
                onValueChanged: {
                    xy_timer.restart()
                    _root.update_Model(-1);
                }
                Timer{
                    id:xy_timer
                    interval: 5000
                    running: false
                    repeat: false
                    onTriggered: _root.show_xy=false
                    onRunningChanged: if(running) _root.show_xy=true
                }
            }
        }
    }


}

