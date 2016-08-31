import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Rectangle {
    id:root
    property var current_item: 0
    onCurrent_itemChanged: {
        if(current_item!=0 &&
            current_item.type==="beam" &&
                tab_view.currentIndex===1)
                tab_view.getTab(1).item.currentIndex=materialsManager.get(current_item.materialID ,"Index");
    }
    color: "transparent"
    TabView{
        id:tab_view
        onCurrentIndexChanged: if(current_item.type==="beam" && currentIndex===1){
                                   tab_view.getTab(1).item.currentIndex=materialsManager.get(current_item.materialID ,"Index");
                               }
        anchors.fill: parent
        style:TabViewStyle {
            tabOverlap: 0
            frame: Rectangle {
                color: "transparent"
            }
            tab:Rectangle {
                color: styleData.selected ? "steelblue" :"#F0F0F0"
                border.color:  color
                implicitWidth: Math.min(text.implicitWidth + 4, styleData.availableWidth)
                implicitHeight: pt2px(14)
                radius: 2
                Text {
                    id: text
                    anchors.centerIn: parent
                    width: Math.min(implicitWidth,parent.width)
                    height: parent.height
                    text: styleData.title
                    color: styleData.selected ? "white" : "black"
                    fontSizeMode: Text.Fit
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter

                }
            }
        }

        Tab{
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
                        //Min between the max allowed and the numeber of objects available
                        height: Math.min(2*parent.height/3, catalog_grid.cellHeight*Math.ceil(catalog_grid.count/2) )
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
                                        text:"Weight:"+warehouse3d.get(index,"weight")+"Kg"
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
                                            logger.log("infobox_designer_remove_load",{"beam":current_item.parent.objectName,"load":current_item.objectName})
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
                                            logger.log("infobox_designer_add_load",{"beam":current_item.objectName,"load_weight":warehouse3d.get(catalog_grid.currentIndex,"weight")})
                                            vmFrame3DDManager.produceTPZForce(current_item,warehouse3d.get(catalog_grid.currentIndex,"properties"))
                                            //current_item=instantiator.createEntity(current_item,"qrc:/element_views/Element_Views/TrapezoidalForce.qml")
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



                        }

                    }
                }
            }
        }
        Tab{
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
                        height: Math.min(2*parent.height/3, materials_list.count*(materialItem.width+20)/3)
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
                                                            current_item.materialID=materialsManager.get(index,"UniqueID")
                                                            logger.log("infobox_designer_change_material",{"beam":current_item.objectName,"materialID":current_item.materialID})
                                                    }
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
                                width: parent.width/2-50

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
                                width: parent.width/2-50
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
                                            current_item.realBeamSize=Qt.size(w.text,h.text);
                                            logger.log("infobox_designer_change_section",{"beam":current_item.objectName,"section": current_item.realBeamSize})
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

}

