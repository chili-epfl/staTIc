import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.2
import QtQuick.Scene3D 2.0
import QtQuick.Layouts 1.1
import "qrc:/ui/UI"

Rectangle {
    id:root
    property var current_item

    property bool hasReaction: false

    onCurrent_itemChanged: {
        forceListModel.clear()
        hasReaction=false;
        infoboxScene3D.resetBeams();
        infoboxScene3D.beam_poses=[];
        if(current_item != 0 && current_item.type=="joint"){
            title.text="Joint "+ current_item.objectName;
            for(var i=0;i<current_item.connected_beams.length;i++){
                forceListModel.append({isAdded:true,
                                       beam:current_item.connected_beams[i],
                                      })
                infoboxScene3D.beam_poses.push(current_item.connected_beams[i].poseMatrix)
            }
            infoboxScene3D.updateBeams();
            //Important that this stays at the end!
            if(Math.abs(current_item.reaction.length())>0.0001){
                   hasReaction=true;
                   forceListModel.append({isAdded:true,
                                          joint:current_item,
                                          })
            }
            updateSumOfForces()
        }
    }
    function updateSumOfForces(){
        var sum=Qt.vector3d(0,0,0)
        var max=computeMaxForce(0);
        for(var i=0;i<current_item.connected_beams.length;i++){
            sum=sum.plus(forceListModel.get(i).beam.globalForceExtreme1.times(forceListModel.get(i).isAdded))
        };
        if(hasReaction){
            sum=sum.plus(forceListModel.get(current_item.connected_beams.length).joint.reaction.times(forceListModel.get(current_item.connected_beams.length).isAdded));
            max=max.length() > max.plus(forceListModel.get(current_item.connected_beams.length).joint.reaction).length() ?
                        max:max.plus(forceListModel.get(current_item.connected_beams.length).joint.reaction)
        }
        infoboxScene3D.currentForce=sum
        infoboxScene3D.maxForce=max;
    }

    function computeMaxForce(index){
        if (index==current_item.connected_beams.length) return Qt.vector3d(0,0,0);
        var next=computeMaxForce(index+1);
        if(next.length() > next.plus(forceListModel.get(index).beam.globalForceExtreme1).length())
            return next;
        else
            return next.plus(forceListModel.get(index).beam.globalForceExtreme1)
    }


    color:"transparent"

    ColumnLayout{
          spacing: 10

        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.05*root.height
            Item{
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    anchors.fill: parent
                    id: title
                    text: qsTr("text")
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10; font.pixelSize: 72
                    color: "white"
                }
            }
        }
        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.70*root.height
            Scene3D {
                anchors.fill: parent
                anchors.margins: 10
                focus: true
                aspects: ["input","logic"]
                multisample: true
                InfoBoxScene3DJoint{
                    id:infoboxScene3D
                }
            }


            //----------Navigation UI-----------------
            Image {
                id: coordinate_system_image
                source: "qrc:/images/Images/coordinate_system.png"
                Binding on source {
                    when: mouseArea.pressed
                    value:"qrc:/images/Images/coordinate_system_pressed.png"
                }
                Binding on source {
                    when: !mouseArea.pressed
                    value:"qrc:/images/Images/coordinate_system.png"
                }
                width: 75
                height: 75
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                MouseArea{
                    id:mouseArea
                    anchors.fill: parent
                    onPressed: infoboxScene3D.resetCamera()
                }
            }

            /*Switch {
                id:controlSwitch
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.horizontalCenter:  parent.horizontalCenter
                height: 35
            }
            Image {
                id: moveIcon
                source: "qrc:/icons/Icons/move.png"
                width: 35
                height: 35
                anchors.rightMargin:  10
                anchors.margins: 30
                anchors.bottom: parent.bottom
                anchors.right: controlSwitch.left
            }
            Image {
                id: panIcon
                source: "qrc:/icons/Icons/pan.png"
                width: 35
                height: 35
                anchors.margins: 30
                anchors.leftMargin: 10
                anchors.bottom: parent.bottom
                anchors.left: controlSwitch.right
            }*/
        }


        Rectangle{
            color:"#2f3439"
            radius: 1
            Layout.preferredWidth: root.width
            Layout.preferredHeight: 0.25*root.height

            ListModel{
                id:forceListModel
            }

            ListView{
                id:forceListView
                clip: true
                anchors.fill: parent
                anchors.margins: 10
                model: forceListModel
                delegate: Item {
                    width: root.width-20
                    height: 2* forceList_switch.height
                    Text {
                        id:forceList_label
                        text: beam ? "Force from beam " + beam.objectName : "Reaction force from support"
                        fontSizeMode: Text.Fit;
                        minimumPixelSize: 10;
                        color: "white"
                        width: parent.width-forceList_switch.width-10
                    }
                    Switch{
                        id:forceList_switch
                        anchors.right: parent.right
                        checked: true
                        style: CustomSwitchStyle{
                        }
                        property vector3d forceVectorDel: (beam) ? beam.globalForceExtreme1 : joint.reaction
                        onForceVectorDelChanged: updateSumOfForces()
                        onCheckedChanged:
                        {
                           forceListModel.setProperty(index, "isAdded", checked)
                           updateSumOfForces()
                        }
                    }
                }
            }

            /*Flickable {
                anchors.fill: parent
                contentWidth: flickableItem.width;
                contentHeight: flickableItem.height;
                clip: true
                flickableDirection: Flickable.VerticalFlick
                rebound: null
                //boundsBehavior:Flickable.StopAtBounds
                Column{
                    id:flickableItem
                    width: root.width
                    height: repeter.model*height/3;
                    Repeater{
                        id:repeter
                        model:10
                        Item {
                            width: parent.width
                            height: 0.25*root.height/3
                            Text {
                                text: qsTr("text")
                            }
                            Switch{
                                anchors.right: parent.right
                            }
                        }
                    }
                }
            }*/



//            Item{
//                anchors.fill: parent
//                anchors.margins: 10
//                visible: (infoboxScene3D.current_selection>=0) ? true : false
//                Text {
//                    id:forceInfo
//                    text: qsTr("Exagerate")
//                    color: "white"
//                }
//                Text {
//                    id:disableLabel
//                    text: qsTr("Disable Force")
//                    color: "white"
//                    anchors.margins: 10
//                    anchors.top:forceInfo.bottom
//                }
//                CheckBox {
//                    id:disableCheckbox
//                    anchors.left: disableLabel.right
//                    anchors.bottom: disableLabel.bottom
//                    onCheckedChanged: {
//                        if(infoboxScene3D.current_selection>=0){
//                             if(checked)
//                                 infoboxScene3D.scale_factors[infoboxScene3D.current_selection] =0;
//                             else
//                                 infoboxScene3D.scale_factors[infoboxScene3D.current_selection] = 1;
//                        }
//                    }
//               }
//            }
        }
    }
}
