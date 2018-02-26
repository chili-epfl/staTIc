import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls 2.3 as QQ2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0
Rectangle {
    id:root
    property var current_item: 0
    onCurrent_itemChanged: {
        if(current_item!=0 &&
                current_item.type==="beam"){
            if(tab_view.currentIndex===1){
                tab_view.getTab(1).item.currentIndex=materialsManager.get(current_item.materialID ,"Index")
            }
            settings.focus_on_joint=false;
        }
        else if(current_item!=0 &&
                current_item.type==="joint"){
            if(tab_view.currentIndex===2){
                if(current_item.supportType=="Pinned")
                    tab_view.getTab(2).item.support_selection="Pinned";
                else  if(current_item.supportType=="Rolling")
                    tab_view.getTab(2).item.support_selection="Rolling Horizontal";
                else  if(current_item.supportType=="Fixed")
                    tab_view.getTab(2).item.support_selection="Fixed";
                else if(current_item.supportType=="none")
                    tab_view.getTab(2).item.support_selection="none";
                settings.focus_on_joint=false;
            }
            else if(tab_view.currentIndex===3)
                settings.focus_on_joint=true;
        }
        else
            settings.focus_on_joint=false;
    }
    color: "transparent"
    TabView{
        id:tab_view
        onCurrentIndexChanged: if(current_item.type==="beam" && currentIndex===1){
                                   tab_view.getTab(1).item.currentIndex=materialsManager.get(current_item.materialID ,"Index");
                                   settings.focus_on_joint=false;
                               }
                               else if(current_item.type==="joint"){
                                   if(currentIndex===2){
                                       if(current_item.supportType=="Pinned")
                                           tab_view.getTab(2).item.support_selection="Pinned";
                                       else  if(current_item.supportType=="Rolling")
                                           tab_view.getTab(2).item.support_selection="Rolling Horizontal";
                                       else  if(current_item.supportType=="Fixed")
                                           tab_view.getTab(2).item.support_selection="Fixed";
                                       else if(current_item.supportType=="none")
                                           tab_view.getTab(2).item.support_selection="none";
                                       settings.focus_on_joint=false;
                                   }
                                   else if(currentIndex===3){
                                       settings.focus_on_joint=true;
                                   }
                               }
                               else{
                                   settings.focus_on_joint=false;
                               }

        anchors.fill: parent
        style:TabViewStyle {
            tabOverlap: 0
            frame: Rectangle {
                color: "transparent"
            }
            tab:Item{
                implicitWidth: styleData.selected ?
                                   styleData.availableWidth/2
                                 :
                                   0.5*styleData.availableWidth/tab_view.count

                implicitHeight: pt2px(14)+30
                Rectangle {
                    color: "#2f3439"
                    border.color:  color
                    anchors.fill: parent
                    anchors.margins: 5
                    radius: 2
                    Rectangle{
                        visible: styleData.selected
                        color: "#33b5e5"
                        width: parent.width
                        height: 5
                        anchors.bottom: parent.bottom
                    }
                    Text {
                        id: text
                        anchors.centerIn: parent
                        width: parent.width
                        height: parent.height-8
                        text: styleData.title
                        color: "#F0F0F0"
                        //                        fontSizeMode: Text.Fit
                        //wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        font.pointSize: 14
                        //                        minimumPointSize: 10

                    }
                }
            }
        }

        Tab{
            id:catalog_tab
            title: "Catalog"
            Rectangle{
                id:catalogItem
                anchors.fill:parent
                color: "transparent"
                Column{
                    anchors.fill: parent
                    spacing: 10
                    Rectangle{
                        border.color: "#F0F0F0"
                        border.width: 5
                        visible: catalog_grid.count > 0
                        width: parent.width
                        //Min between the max allowed and the number of objects available
                        height: Math.min(2*parent.height/3-gravity_modifier.height, 10+catalog_grid.cellHeight*Math.ceil(catalog_grid.count/2) )
                        anchors.margins: 10
                        color:"transparent"
                        radius: 1
                        GridView{
                            id:catalog_grid
                            anchors.fill: parent;
                            anchors.margins: 10
                            model: warehouse3d
                            clip: true
                            cellWidth : parent.width/2-10
                            cellHeight: cellWidth
                            delegate: Rectangle{
                                height: catalog_grid.cellHeight-10
                                width:  catalog_grid.cellWidth-10
                                color:"transparent";
                                Image {
                                    id:image
                                    width: parent.width
                                    height: 0.9*parent.height
                                    anchors.fill: parent
                                    anchors.margins: 20
                                    source: decoration
                                }
                                Rectangle{
                                    color: "#2f3439"
                                    width: parent.width
                                    anchors.leftMargin : 20
                                    anchors.rightMargin: 20
                                    height: 0.1*parent.height
                                    anchors.top: image.bottom
                                    Text{
                                        anchors.centerIn: parent
                                        height: parent.height
                                        color: "#F0F0F0"
                                        text:whatsThis!="uniform"? "Weight:"+warehouse3d.get(index,"weight")+"Kg" :  "Load:"+warehouse3d.get(index,"weight")+"Kg/mm"
                                        fontSizeMode: Text.Fit
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: catalog_grid.currentIndex=index;
                                }
                            }
                            highlight: Rectangle { color: "red"; radius: 5 }
                        }
                    }
                    /*Apply/Delete modifier*/
                    Rectangle{
                        border.color: "#F0F0F0"
                        border.width: 5
                        width: parent.width
                        height: parent.width/3
                        anchors.margins: 10
                        color: "transparent"
                        Rectangle{
                            color:"#2f3439";
                            anchors.margins: 10
                            anchors.fill: parent
                            Image {
                                id: object_delete_icon
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.horizontalCenter
                                width: Math.min((parent.width-20)/2,parent.height)
                                height: width
                                source: object_delete_button.pressed ? "qrc:/icons/Icons/delete_load_pressed.png":"qrc:/icons/Icons/delete_load.png"
                                MouseArea{
                                    id:object_delete_button
                                    anchors.fill: parent
                                    onClicked:{
                                        if(current_item.type==="trapezoidalForce"){
                                            //logger.log("infobox_designer_remove_load",{"beam":current_item.parent.objectName,"load":current_item.objectName})
                                            current_item.killMe();
                                            infobox.current_item=0;
                                        }else if(current_item.type==="nodeForce"){
                                            //logger.log("infobox_designer_remove_load",{"joint":current_item.parent.objectName,"load":current_item.objectName})
                                            current_item.killMe();
                                            infobox.current_item=0;
                                        }
                                        else if(current_item.type==="uniformForce"){
                                            //logger.log("infobox_designer_remove_load",{"beam":current_item.parent.objectName,"load":current_item.objectName})
                                            current_item.killMe();
                                            infobox.current_item=0;
                                        }
                                        else{
                                            suggestion_box.show_message("You need to select a load first")
                                        }
                                    }
                                }
                            }
                            Image {
                                id: object_apply_icon
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: object_delete_icon.right
                                width: Math.min((parent.width-20)/2,parent.height)
                                height: width
                                source: object_apply_button.pressed ? "qrc:/icons/Icons/apply_load_pressed.png":"qrc:/icons/Icons/apply_load.png"
                                MouseArea{
                                    id:object_apply_button
                                    anchors.fill: parent
                                    onClicked:{
                                        if(current_item.type=="beam"){
                                            if(warehouse3d.get(catalog_grid.currentIndex,"type")!="uniform"){
                                                //logger.log("infobox_designer_add_load",{"beam":current_item.objectName,"load_weight":warehouse3d.get(catalog_grid.currentIndex,"weight")})
                                                staticsModule.createTPZLoad(current_item.backend_entity,{"parent_entity":current_item,"warehouse_index":catalog_grid.currentIndex})
                                            }else{
                                                //logger.log("infobox_designer_add_load",{"beam":current_item.objectName,"load_weight":warehouse3d.get(catalog_grid.currentIndex,"weight"),"type":"uniform"})
                                                staticsModule.createUDLoad(current_item.backend_entity,{"parent_entity":current_item,"warehouse_index":catalog_grid.currentIndex})
                                            }
                                        }

                                        else if(current_item.type=="joint"){
                                            if(warehouse3d.get(catalog_grid.currentIndex,"type")!="uniform"){
                                                //logger.log("infobox_designer_add_load",{"joint":current_item.objectName,"load_weight":warehouse3d.get(catalog_grid.currentIndex,"weight")})
                                                staticsModule.createNodeLoad(current_item.backend_entity,{"parent_entity":current_item,"warehouse_index":catalog_grid.currentIndex})
                                            }
                                        }
                                        else if(current_item.type=="trapezoidalForceTangible"){
                                            current_item.weight=warehouse3d.get(catalog_grid.currentIndex,"weight")
                                            current_item.extent=warehouse3d.get(catalog_grid.currentIndex,"extent")
                                            current_item.asset3DMeshURL=warehouse3d.get(catalog_grid.currentIndex,"main_asset_url")
                                            current_item.asset3DTextureURL=warehouse3d.get(catalog_grid.currentIndex,"main_asset_diffuse_map_url")
                                        }
                                        else{
                                            suggestion_box.show_message("You need to select a beam first")
                                        }
                                    }
                                }
                            }
                            Image {
                                source: "qrc:/icons/Icons/help.png"
                                fillMode: Image.PreserveAspectFit
                                width: mm2px(10)
                                height: width
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: suggestion_box.show_message("To add a load, select the load, click on the target beam or joint and press the button \"Apply\".\n To remove a load, click on it and press the button \"Remove\".\n To move a load along a beam, click the load so that several green spheres appear. Click on a sphere to set the load in that position. ");
                                }
                            }


                        }
                    }
                    Rectangle{
                        id:gravity_modifier
                        border.color: "#F0F0F0"
                        color:"transparent"
                        border.width: 5
                        width: parent.width
                        height: gravity_check_box.implicitHeight+30
                        Rectangle{
                            color:"#F0F0F0";
                            anchors.margins: 10
                            anchors.fill: parent
                            CheckBox{
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                id:gravity_check_box
                                text:"Apply Gravity"
                                checked: false
                                onCheckedChanged: {
                                    checked ?
                                                      staticsModule.gravity=Qt.vector3d(0,-9800,0)
                                                      :staticsModule.gravity=Qt.vector3d(0,0,0)}
                                anchors.margins: 10
                            }
                        }
                    }

                }
            }
        }
        Tab{
            id:material_tab
            title:"Beam Materials"
            Rectangle{
                id:materialItem
                property alias currentIndex: materials_list.currentIndex
                anchors.fill:parent
                color: "transparent"

                Column{
                    anchors.fill: parent
                    spacing: 10
                    Rectangle{
                        border.color: "#F0F0F0"
                        border.width: 5
                        visible: materials_list.count > 0
                        width: parent.width
                        //Min between the max allowed and the numeber of materials available
                        height: Math.min(2*parent.height/3 - 100, materials_list.count*(materialItem.width+20)/3)
                        anchors.margins: 10
                        color:"transparent"
                        radius: 1
                        ListView{
                            id:materials_list
                            focus: true
                            anchors.fill: parent;
                            anchors.margins: 10
                            model: materialsManager
                            clip: true
                            spacing: 10
                            delegate: Rectangle{
                                width: materialItem.width-20
                                height: (materialItem.width-20)/3
                                color:"#2f3439";
                                SequentialAnimation on color{
                                    id:feedback_animation_material
                                    running: false
                                    ColorAnimation{
                                        to: "green"
                                        duration: 750
                                    }
                                    ColorAnimation{
                                        to: "#2f3439"
                                        duration: 750
                                    }
                                }
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: materials_list.currentIndex=index;
                                }
                                RowLayout{
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10
                                    Image {
                                        source:decoration
                                        Layout.preferredWidth:  height
                                        Layout.preferredHeight: parent.height
                                    }
                                    Rectangle{
                                        color:materials_list.currentIndex==index ?
                                                  "red" : "transparent";
                                        Layout.preferredWidth:  10
                                        Layout.preferredHeight: parent.height
                                    }
                                    Rectangle{
                                        color:"#2f3439"
                                        Layout.maximumWidth:  parent.width-parent.height-10
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: parent.height
                                        ColumnLayout{
                                            anchors.fill: parent
                                            Layout.margins: 5
                                            Text {
                                                Layout.fillHeight: true
                                                Layout.fillWidth: true
                                                color: "white"
                                                text: display
                                                fontSizeMode: Text.Fit
                                                font.pointSize: 12
                                            }
                                            Text {
                                                Layout.fillHeight: true
                                                Layout.fillWidth: true
                                                color: "white"
                                                text: "Price:"+materialsManager.get(index,"Price")
                                                fontSizeMode: Text.Fit
                                                font.pointSize: 12

                                            }
                                            Text {
                                                Layout.fillHeight: true
                                                Layout.fillWidth: true
                                                color: "white"
                                                text: "Density:"+materialsManager.get(index,"Density").toLocaleString()
                                                fontSizeMode: Text.Fit
                                                font.pointSize: 12

                                            }

                                        }
                                        Image{
                                            visible: materials_list.currentIndex==index
                                            anchors.bottom: parent.bottom
                                            anchors.right: parent.right
                                            height: 50
                                            width: 50
                                            source: material_apply_button.pressed ? "qrc:/icons/Icons/apply_pressed.png":"qrc:/icons/Icons/apply.png"
                                            MouseArea{
                                                id:material_apply_button
                                                anchors.fill: parent
                                                onClicked:{
                                                    if(current_item!=0 && current_item.type=="beam"){
                                                        feedback_animation_material.start();
                                                        current_item.setMaterialID(materialsManager.get(index,"UniqueID"))
                                                        //logger.log("infobox_designer_change_material",{"beam":current_item.objectName,"materialID":current_item.materialID})
                                                    }
                                                }
                                            }

                                        }
                                        Image{
                                            visible: materials_list.currentIndex==index
                                            anchors.top: parent.top
                                            anchors.right: parent.right
                                            height: 50
                                            width: 50
                                            source: material_apply_all_button.pressed ? "qrc:/icons/Icons/apply_all_pressed.png":"qrc:/icons/Icons/apply_all.png"
                                            MouseArea{
                                                id:material_apply_all_button
                                                anchors.fill: parent
                                                onClicked:{
                                                    var beams=sceneRoot.findBeams()
                                                    for(var beam_index=0;beam_index<beams.length;beam_index++){
                                                        beams[beam_index].setMaterialID(materialsManager.get(index,"UniqueID"))
                                                    }
                                                    feedback_animation_material.start();
                                                    //logger.log("infobox_designer_change_material_global",{"materialID":current_item.materialID})

                                                }
                                            }

                                        }
                                    }
                                }

                            }
                        }
                    }
                    /*Size modifier*/
                    Rectangle{
                        border.color: "#F0F0F0"
                        border.width: 5
                        width: parent.width
                        height: parent.width/3
                        anchors.margins: 10
                        color: "transparent"
                        Rectangle{
                            color:"#2f3439";
                            anchors.margins: 10
                            anchors.fill: parent
                            SequentialAnimation on color{
                                id:feedback_animation_size
                                running: false
                                ColorAnimation{
                                    to: "green"
                                    duration: 750
                                }
                                ColorAnimation{
                                    to: "#2f3439"
                                    duration: 750
                                }
                            }
                            Text {
                                id:caption_size
                                color: "white"
                                text: "Size"
                                anchors.margins: 10
                                fontSizeMode: Text.Fit
                                minimumPixelSize: 10; font.pixelSize: 25
                                anchors.top:parent.top
                                anchors.left: parent.left

                            }
                            TextField{
                                id:w
                                focus:false
                                activeFocusOnTab: false
                                anchors.verticalCenter: x.verticalCenter
                                anchors.right: x.left
                                anchors.rightMargin: 25
                                text: current_item!=0 &&
                                      current_item.type =="beam" ? current_item.realBeamSize.width:100
                                maximumLength: 5
                                validator: DoubleValidator {bottom: 1; top: 50000;}
                                width: parent.width/2-100

                            }
                            Text{
                                id:x
                                text:"X"
                                color: "white"
                                anchors.centerIn: parent
                                fontSizeMode: Text.Fit
                                minimumPixelSize: 10; font.pixelSize: 25
                            }

                            TextField{
                                id:h
                                focus:false
                                activeFocusOnTab: false
                                maximumLength: 5
                                anchors.verticalCenter: x.verticalCenter
                                anchors.left: x.right
                                anchors.leftMargin: 25
                                width: parent.width/2-100
                                text:current_item!=0 &&
                                     current_item.type=="beam"?current_item.realBeamSize.height:100
                                validator: DoubleValidator {bottom: 1; top: 50000;}
                            }
                            Text{
                                fontSizeMode: Text.Fit
                                color: "white"
                                minimumPixelSize: 10; font.pixelSize: 25
                                text: "W(mm)"
                                anchors.horizontalCenter: w.horizontalCenter
                                anchors.top: w.bottom
                                anchors.topMargin: 10
                            }
                            Text{
                                fontSizeMode: Text.Fit
                                color: "white"
                                minimumPixelSize: 10; font.pixelSize: 25
                                text: "H(mm)"
                                anchors.horizontalCenter: h.horizontalCenter
                                anchors.top: h.bottom
                                anchors.topMargin: 10
                            }
                            Image{
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                height: 50
                                width: 50
                                source: size_apply_button.pressed ? "qrc:/icons/Icons/apply_pressed.png":"qrc:/icons/Icons/apply.png"
                                MouseArea{
                                    id:size_apply_button
                                    anchors.fill: parent
                                    onClicked:{
                                        if(current_item!=0 && current_item.type=="beam"){
                                            feedback_animation_size.start();
                                            current_item.setBeamSize(Qt.size(w.text,h.text));
                                            //logger.log("infobox_designer_change_section",{"beam":current_item.objectName,"section": current_item.realBeamSize})
                                        }
                                    }
                                }
                            }
                            Image{
                                anchors.top: parent.top
                                anchors.right: parent.right
                                height: 50
                                width: 50
                                source: size_apply_button.pressed ? "qrc:/icons/Icons/apply_all_pressed.png":"qrc:/icons/Icons/apply_all.png"
                                MouseArea{
                                    id:size_apply_all_button
                                    anchors.fill: parent
                                    onClicked:{
                                        var beams=sceneRoot.findBeams()
                                        for(var beam_index=0;beam_index<beams.length;beam_index++){
                                            beams[beam_index].setBeamSize(Qt.size(w.text,h.text));
                                        }
                                        feedback_animation_size.start();
                                        //logger.log("infobox_designer_change_section_all",{"section": current_item.realBeamSize})

                                    }
                                }

                            }

                        }
                    }
                    Rectangle{
                        border.color: "#F0F0F0"
                        border.width: 5
                        width: parent.width
                        height: 100
                        anchors.margins: 10
                        color: "transparent"
                        Rectangle{
                            color:"#F0F0F0";
                            anchors.margins: 10
                            anchors.fill: parent
                            CheckBox {
                                id:beam_enable_check
                                enabled: current_item!=0 && current_item.type=="beam"
                                onClicked:  current_item.backend_entity.enabled = checked
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                anchors.left: parent.left
                                text:"Enable/Disable"
                                Component.onCompleted:{
                                    beam_enable_check.checked= enabled && current_item.backend_entity.enabled
                                }
                                Connections{
                                    target:root
                                    ignoreUnknownSignals: true
                                    onCurrent_itemChanged: if(current_item!=0 && current_item.type=="beam")
                                                               beam_enable_check.checked=current_item.backend_entity.enabled
                                }
                                Connections {
                                    target: current_item!=0 ? current_item.backend_entity : null
                                    ignoreUnknownSignals: true
                                    onEnabledChanged: {
                                        beam_enable_check.checked = current_item.backend_entity.enabled
                                        //logger.log("infobox_designer_enable_beam",{"beam":current_item.objectName,"enabled": beam_enable_check.checked})
                                    }
                                }
                            }
                            Image {
                                source: "qrc:/icons/Icons/help.png"
                                fillMode: Image.PreserveAspectFit
                                width: mm2px(10)
                                height: width
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: suggestion_box.show_message("To change the properties of a beam, you have to click on it first and then apply your changes.");
                                }
                            }
                        }
                    }
                }

            }

        }
        Tab{
            id:joint_design
            title:"Joint Design"
            Item{
                property alias support_selection: support_designer_rect.selection
                anchors.fill: parent
                Rectangle{
                    id:support_design_joint_name
                    border.color: "#F0F0F0"
                    border.width: 5
                    color: "#2f3439"
                    Text {
                        color: "white"
                        anchors.fill: parent
                        anchors.margins: 10
                        text: current_item!=0 && current_item.type=="joint"? "Joint "+ current_item.objectName : "Select a joint"
                        font.pointSize: 30
                        fontSizeMode: Text.Fit
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    width: parent.width
                    height: parent.height*0.07
                    anchors.margins: 10

                }

                Rectangle{
                    anchors.top: support_design_joint_name.bottom
                    id:support_designer_rect
                    border.color: "#F0F0F0"
                    border.width: 5
                    color: "#2f3439"
                    width: parent.width
                    height: parent.height*0.8
                    anchors.margins: 10

                    property string selection:"none";

                    GridLayout{
                        columns: 3
                        rows: 4
                        anchors.fill: parent
                        anchors.margins: 10
                        Text{
                            text:"Name"
                            color:"white"
                            font.bold: true
                            font.pointSize: 12
                            Layout.preferredWidth: 1/4*support_designer_rect.width
                            Layout.alignment: Qt.AlignHCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text{
                            text:"Symbol"
                            color:"white"
                            font.bold: true
                            font.pointSize: 12
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text{
                            text:"Reaction"
                            color:"white"
                            font.bold: true
                            font.pointSize: 12
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            horizontalAlignment: Text.AlignHCenter

                        }

                        Rectangle{
                            visible: true
                            color: "transparent"
                            border.color: support_designer_rect.selection=="Fixed"? "red" : "transparent"
                            border.width: 5
                            Layout.preferredWidth: 1/4*support_designer_rect.width
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4
                            Text{
                                text:"Fixed"
                                color:"white"
                                font.pointSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                anchors.fill: parent
                                anchors.margins: 10
                                fontSizeMode: Text.Fit
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    if(support_designer_rect.selection=="Fixed")
                                        support_designer_rect.selection="none"
                                    else
                                        support_designer_rect.selection="Fixed"
                                    if(current_item!=0 && current_item.type==="joint")
                                        current_item.setSupportType(support_designer_rect.selection)
                                }
                            }
                        }

                        Image{
                            visible: true
                            source: "qrc:/images/Images/fix_support.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4

                        }
                        Image{
                            visible: true
                            source: "qrc:/images/Images/fix_support_react.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4
                        }

                        Rectangle{
                            color: "transparent"
                            border.color: support_designer_rect.selection=="Pinned"? "red" : "transparent"
                            border.width: 5
                            Layout.preferredWidth: 1/4*support_designer_rect.width
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4
                            Text{
                                text:"Pinned"
                                color:"white"
                                font.pointSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                anchors.fill: parent
                                anchors.margins: 10
                                fontSizeMode: Text.Fit
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked:  {
                                    if(support_designer_rect.selection=="Pinned")
                                        support_designer_rect.selection="none"
                                    else
                                        support_designer_rect.selection="Pinned"
                                    if(current_item!=0 && current_item.type==="joint")
                                        current_item.setSupportType(support_designer_rect.selection)
                                }
                            }
                        }
                        Image{
                            source: "qrc:/images/Images/pinned_support.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4

                        }
                        Image{
                            source: "qrc:/images/Images/pinned_support_react.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4

                        }
                        Rectangle{
                            color: "transparent"
                            border.color: support_designer_rect.selection=="Rolling Horizontal"? "red" : "transparent"
                            border.width: 5
                            Layout.preferredWidth: 1/4*support_designer_rect.width
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4
                            Text{
                                text:"Rolling Horizontal"
                                color:"white"
                                font.pointSize: 11
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                anchors.fill: parent
                                anchors.margins: 10
                                fontSizeMode: Text.Fit
                                wrapMode: Text.WordWrap
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    if(support_designer_rect.selection=="Rolling Horizontal")
                                        support_designer_rect.selection="none"
                                    else
                                        support_designer_rect.selection="Rolling Horizontal"
                                    if(current_item!=0 && current_item.type==="joint")
                                        if(support_designer_rect.selection!="none")
                                            current_item.setSupportType("Rolling")
                                        else
                                            current_item.setSupportType(support_designer_rect.selection)
                                }
                            }
                        }
                        Image{
                            source: "qrc:/images/Images/rolling_support.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4

                        }
                        Image{
                            source: "qrc:/images/Images/rolling_support_react.png"
                            Layout.fillWidth: true
                            fillMode: Image.PreserveAspectFit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: support_designer_rect.height/4
                        }

                    }
                }
            }
        }
        Tab{
            id:joint_analysis
            title:"Joint Analysis"


            Item{
                property bool hasReaction:false;
                Component.onCompleted: updateModel()

                Connections{
                    target: root
                    onCurrent_itemChanged: {
                        updateModel()
                    }
                }
                Connections{
                    target: applicationRoot.staticsModule
                    onUpdated: {
                        updateModel()
                    }
                }

                function updateModel(){
                    forceListModel.clear()
                    hasReaction=false;
                    if(current_item != 0 && current_item.type=="joint"){
                        for(var i=0;i<current_item.connected_beams_instatiator.count;i++){
                            forceListModel.append({isAdded:false,
                                                      beam:current_item.connected_beams_instatiator.objectAt(i),
                                                  })
                        }
                        //Important that this stays at the end!
                        if(Math.abs(current_item.reaction.length())>0.0001){
                            hasReaction=true;
                            forceListModel.append({isAdded:false,
                                                      joint:current_item,
                                                  })
                        }
                        updateSumOfForces()
                        //logger.log("infobox_joint_item_changed",{"item":current_item.objectName})
                    }
                }


                function updateSumOfForces(){
                    var sum=Qt.vector3d(0,0,0)
                    var max=computeMaxForce(0);
                    var logging_fields={};
                    for(var i=0;i<current_item.connected_beams_instatiator.count;i++){
                        sum=sum.plus(forceListModel.get(i).beam.globalForceExtreme1.times(forceListModel.get(i).isAdded))
                        logging_fields[forceListModel.get(i).beam.objectName]=forceListModel.get(i).isAdded;
                    };
                    if(hasReaction){
                        sum=sum.plus(forceListModel.get(current_item.connected_beams_instatiator.count).joint.reaction.times(forceListModel.get(current_item.connected_beams_instatiator.count).isAdded));
                        max=max.length() > max.plus(forceListModel.get(current_item.connected_beams_instatiator.count).joint.reaction).length() ?
                                    max:max.plus(forceListModel.get(current_item.connected_beams_instatiator.count).joint.reaction)

                        logging_fields[forceListModel.get(current_item.connected_beams_instatiator.count).joint.objectName]=forceListModel.get(current_item.connected_beams_instatiator.count).isAdded;

                    }
                    settings.focus_view_currentForce=sum
                    settings.focus_view_maxForce=max;
                }

                function computeMaxForce(index){
                    if (index==current_item.connected_beams_instatiator.count) return Qt.vector3d(0,0,0);
                    var next=computeMaxForce(index+1);
                    if(next.length() > next.plus(forceListModel.get(index).beam.globalForceExtreme1).length())
                        return next;
                    else
                        return next.plus(forceListModel.get(index).beam.globalForceExtreme1)
                }

                anchors.fill: parent
                Rectangle{
                    id:joint_analysis_joint_name
                    border.color: "#F0F0F0"
                    border.width: 5
                    color: "#2f3439"
                    Text {
                        color: "white"
                        anchors.fill: parent
                        anchors.margins: 10
                        text: current_item!=0 && current_item.type=="joint"? "Joint "+ current_item.objectName : "Select a joint"
                        font.pointSize: 30
                        fontSizeMode: Text.Fit
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    width: parent.width
                    height: parent.height*0.07
                    anchors.margins: 10

                }

                Rectangle{
                    anchors.top: joint_analysis_joint_name.bottom
                    border.color: "#F0F0F0"
                    border.width: 5
                    color: "#2f3439"
                    width: parent.width
                    height: parent.height*0.8
                    anchors.margins: 10
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
                                anchors.verticalCenter: forceList_switch.verticalCenter
                            }
                            Switch{
                                id:forceList_switch
                                anchors.right: parent.right
                                checked: false
                                width: height*2
                                height: forceList_label.height*1.5
                                style: CustomSwitchStyle{
                                }
                                property vector3d forceVectorDel: (beam) ? beam.globalForceExtreme1 : joint.reaction
                                onForceVectorDelChanged: updateSumOfForces()
                                onCheckedChanged:
                                {
                                    forceListModel.setProperty(index, "isAdded", checked)
                                    updateSumOfForces()
                                    var logging_fields={};
                                    for(var i=0;i<current_item.connected_beams.length;i++){
                                        logging_fields[forceListModel.get(i).beam.objectName]=forceListModel.get(i).isAdded;
                                    };
                                    if(hasReaction){
                                        logging_fields[forceListModel.get(current_item.connected_beams.length).joint.objectName]=forceListModel.get(current_item.connected_beams.length).isAdded;
                                    }
                                    //logger.log("infobox_joint_change_force_list",logging_fields);
                                }
                            }
                        }
                    }

                }
            }
        }
        Tab{
            id:poseOffsetTab
            title: "Adjust AR"
            focus: true
            Rectangle{
                id:ar_settings_box
                border.color: "#F0F0F0"
                border.width: 5
                color: "#2f3439"
                anchors.fill: parent
                anchors.margins: 10
                property vector3d translation:sceneRoot.translation
                Binding{
                    target: sceneRoot
                    property: "translation"
                    value:ar_settings_box.translation
                }
                property vector3d eulerAngles:sceneRoot.eulerAngles
                Binding{
                    target: sceneRoot
                    property: "eulerAngles"
                    value:ar_settings_box.eulerAngles
                }
                Flickable{
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    contentWidth: parent.width
                    contentHeight: ar_setting_column.implicitHeight
                    flickableDirection: Flickable.VerticalFlick
                    Column{
                        id:ar_setting_column
                        width: parent.width-20
                        spacing: mm2px(5)
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_x.implicitHeight,spinbox_x.implicitHeight)
                            QQ2.Label{
                                id:label_spinbox_x
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Offset X (mm)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                focus:false
                                activeFocusOnTab: false
                                id:spinbox_x
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.right: parent.right
                                anchors.margins: 10
                                from:-1000
                                to:1000
                                value: ar_settings_box.translation.x
                                onValueChanged: ar_settings_box.translation.x=value;

//                                validator: IntValidator {
//                                    bottom: Math.min(parent.from, parent.to)
//                                    top:  Math.max(parent.from, parent.to)
//                                }
                                editable: true

                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_y.implicitHeight,spinbox_y.implicitHeight)
                            QQ2.Label{
                                id:label_spinbox_y
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Offset Y (mm)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                anchors.right: parent.right
                                id:spinbox_y
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from:-1000
                                to:1000
//                                validator: IntValidator {
//                                    bottom: Math.min(parent.from, parent.to)
//                                    top:  Math.max(parent.from, parent.to)
//                                }
                                editable: true
                                value: ar_settings_box.translation.y
                                onValueChanged: ar_settings_box.translation.y=value;
                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_z.implicitHeight,spinbox_z.implicitHeight)
                            QQ2.Label{
                                id:label_spinbox_z
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Offset Z (mm)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                id:spinbox_z
                                anchors.right: parent.right
                                focus:false
                                activeFocusOnTab: false
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from:-1000
                                to:1000
//                                validator: IntValidator {
//                                    bottom: Math.min(parent.from, parent.to)
//                                    top:  Math.max(parent.from, parent.to)
//                                }
                                editable: true
                                value: ar_settings_box.translation.z
                                onValueChanged: ar_settings_box.translation.z=value;
                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_rot_x.implicitHeight,spinbox_rotx.implicitHeight)

                            QQ2.Label{
                                id:label_spinbox_rot_x
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Rotation X (degrees)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                focus:false
                                anchors.right: parent.right
                                activeFocusOnTab: false
                                id:spinbox_rotx
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from: -1800
                                to: 1800
                                stepSize: 1

                                property int decimals: 1
                                property real realValue: value / 10

                                onRealValueChanged: ar_settings_box.eulerAngles.x=realValue;
                                value:ar_settings_box.eulerAngles.x*10
                                onValueModified: value=value//break binding

                                validator: DoubleValidator {
                                    bottom: Math.min(parent.from, parent.to)
                                    top:  Math.max(parent.from, parent.to)
                                }

                                textFromValue: function(value, locale) {
                                    return Number(value / 10).toLocaleString(locale, 'f', decimals)
                                }

                                valueFromText: function(text, locale) {
                                    return Number.fromLocaleString(locale, text) * 10
                                }
                                editable: true

                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_rot_y.implicitHeight,spinbox_roty.implicitHeight)

                            QQ2.Label{
                                id: label_spinbox_rot_y
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Rotation Y (degrees)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                focus:false
                                anchors.right: parent.right

                                activeFocusOnTab: false
                                id: spinbox_roty
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from: -1800
                                to: 1800
                                stepSize: 1

                                property int decimals: 1
                                property real realValue: value / 10
                                onRealValueChanged: ar_settings_box.eulerAngles.y=realValue;
                                value:ar_settings_box.eulerAngles.y*10
                                onValueModified: value=value//break binding

                                validator: DoubleValidator {
                                    bottom: Math.min(parent.from, parent.to)
                                    top:  Math.max(parent.from, parent.to)
                                }

                                textFromValue: function(value, locale) {
                                    return Number(value / 10).toLocaleString(locale, 'f', decimals)
                                }

                                valueFromText: function(text, locale) {
                                    return Number.fromLocaleString(locale, text) * 10
                                }
                                editable: true

                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_spinbox_rot_z.implicitHeight,spinbox_rotz.implicitHeight)

                            QQ2.Label{
                                id:label_spinbox_rot_z
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Rotation Z (degrees)"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                anchors.right: parent.right
                                focus:false
                                activeFocusOnTab: false
                                id:spinbox_rotz
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from: -1800
                                to: 1800
                                stepSize: 1

                                property int decimals: 1
                                property real realValue: value / 10

                                onRealValueChanged: ar_settings_box.eulerAngles.z=realValue;
                                value:ar_settings_box.eulerAngles.z*10
                                onValueModified: value=value//break binding

                                validator: DoubleValidator {
                                    bottom: Math.min(parent.from, parent.to)
                                    top:  Math.max(parent.from, parent.to)
                                }

                                textFromValue: function(value, locale) {
                                    return Number(value / 10).toLocaleString(locale, 'f', decimals)
                                }

                                valueFromText: function(text, locale) {
                                    return Number.fromLocaleString(locale, text) * 10
                                }
                                editable: true


                            }
                        }
                        Item{
                            width: parent.width
                            height:Math.max(label_modelscale.implicitHeight,spinbox_modelScale.implicitHeight)

                            QQ2.Label{
                                id:label_modelscale
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 10
                                text: "Model Scale"
                                font.bold: true
                                color:"#F0F0F0"
                                wrapMode:QQ2.Label.WordWrap
                                anchors.left: parent.left
                                anchors.right: parent.horizontalCenter
                            }
                            QQ2.SpinBox{
                                anchors.right: parent.right
                                focus:false
                                activeFocusOnTab: false
                                id:spinbox_modelScale
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.horizontalCenter
                                anchors.margins: 10
                                from: 1
                                to: 100
                                stepSize: 1

                                property int decimals: 2
                                property real realValue: value / 100
                                onRealValueChanged: staticsModule.modelScale=realValue;
                                value:staticsModule.modelScale*100
                                onValueModified: value=value//break binding

                                validator: DoubleValidator {
                                    bottom: Math.min(parent.from, parent.to)
                                    top:  Math.max(parent.from, parent.to)
                                }

                                textFromValue: function(value, locale) {
                                    return Number(value / 100).toLocaleString(locale, 'f', decimals)
                                }

                                valueFromText: function(text, locale) {
                                    return Number.fromLocaleString(locale, text) * 100
                                }
                                editable: true
                            }
                        }
                        Item{
                            width: parent.width
                            height:label_labeling_threshold.implicitHeight+labeling_threshold.implicitHeight+
                                   label_sun.implicitHeight+40
                            QQ2.Label{
                                id:label_labeling_threshold
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.margins: 10
                                text: "Light Condition"
                                font.bold: true
                                color:"#F0F0F0"
                                QQ2.Slider{
                                    id:labeling_threshold
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.bottom
                                    anchors.margins: 10
                                    value: 150

                                    from: 30
                                    to: 220
                                    stepSize: 5

                                    Binding {
                                        target: marker_detector
                                        property: "labelingThreshold"
                                        value: labeling_threshold.value
                                    }
                                    QQ2.Label{
                                        id:label_sun
                                        text:"\u263C"
                                        anchors.top:parent.bottom
                                        font.bold: true
                                        color:"#F0F0F0"
                                        anchors.margins: 5
                                    }
                                    QQ2.Label{
                                        text:"\u2600"
                                        anchors.top:parent.bottom
                                        anchors.right: parent.right
                                        font.bold: true
                                        color:"#F0F0F0"
                                        anchors.margins: 5
                                    }
                                }
                            }
                        }
                    }

            }
            }

        }
    }

}

