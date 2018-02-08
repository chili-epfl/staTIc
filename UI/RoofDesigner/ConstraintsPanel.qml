import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Rectangle {
    id: constraintsPanel
    color: "#999999"

    property var selected_constraint

    Drag.active: drag_area.drag.active

    onVisibleChanged: {
        listEntities.clear()
        horz_const.enabled = false
        vert_const.enabled = false
        leng_const.enabled = false
        equL_const.enabled = false
        dist_const.enabled = false
        para_const.enabled = false
        perp_const.enabled = false
        angl_const.enabled = false
        midP_const.enabled = false

    }

    Rectangle{
        color: "white"
        border.color: "black"
        border.width: 2
        Text {
            anchors.fill: parent
            text: "\uf047"
            font.family: "FontAwesome"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        anchors.verticalCenter: parent.verticalCenter
        width: Screen.pixelDensity*5
        height: width
        anchors.right: parent.left
        radius: width/2
        MouseArea{
            id:drag_area
            anchors.fill: parent
            drag.target: constraintsPanel
            drag.onActiveChanged: {
                if(!drag.active){
                    var x_anchor=constraintsPanel.x
                    var y_anchor=constraintsPanel.y+0.5*constraintsPanel.height
                    if(x_anchor<Screen.pixelDensity*10 ||
                            x_anchor>constraintsPanel.parent.width-Screen.pixelDensity*10 ||
                            y_anchor<Screen.pixelDensity*10 ||
                            y_anchor>constraintsPanel.parent.height-Screen.pixelDensity*10
                            ){
                        constraintsPanel.x=constraintsPanel.parent.width
                        constraintsPanel.y=constraintsPanel.parent.height/2-constraintsPanel.height/2
                    }
                }

            }
        }
    }
    property ListModel listEntities: ListModel{
        onCountChanged: {}
    }
    property alias constrains_list_model: constrains_list_model

    property bool switch_views: true
    Item{
        anchors.right: parent.left
        anchors.top: parent.top
        transformOrigin: "BottomRight"
        rotation: -90
        width: parent.height/3
        height: Screen.pixelDensity*5
        RowLayout{
            anchors.fill: parent
            Rectangle{
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: switch_views ? "black" : "white"
                Text{
                    text:"Add"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    fontSizeMode: Text.Fit
                    color: switch_views ? "white" : "black"
                    anchors.fill: parent
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: switch_views=true
                }

            }
            Rectangle{
                color: !switch_views ? "black" : "white"
                Layout.fillHeight: true
                Layout.fillWidth: true
                Text{
                    text:"Edit"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    fontSizeMode: Text.Fit
                    color: !switch_views ? "white" : "black"
                    anchors.fill: parent
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: switch_views=false
                }
            }
        }
    }


    property alias horz_const: horz_const
    property alias vert_const: vert_const
    property alias leng_const: leng_const
    property alias equL_const: equL_const
    property alias dist_const: dist_const
    property alias para_const: para_const
    property alias perp_const: perp_const
    property alias angl_const: angl_const
    property alias midP_const: midP_const

    Item{
        anchors.fill: parent
        anchors.margins: 10
        visible: !switch_views
        onVisibleChanged: {
            if(visible){
                constrains_list_model.update_model()
            }
        }

        ListModel{
            id:constrains_list_model

            function update_model(){
                constrains_list_model.clear()
                for(var i=0;i<sketch.children.length;i++){
                    if(sketch.children[i].class_type=="Constraint" && sketch.children[i].existing){
                        constrains_list_model.append({"constraint":sketch.children[i],"selected":false})
                    }
                }
            }
        }
        Connections{
            target: sketch
            onUndo:{
                constrains_list_model.clear()
                update_timer.start()
            }
            onRedo: {
                constrains_list_model.clear()
                update_timer.start()
            }
        }

        Timer{
            id:update_timer
            interval: 200
            running: false
            onTriggered: constrains_list_model.update_model()

        }
        ListView {
            id:constrains_list_view
            anchors.fill: parent
            clip: true
            model: constrains_list_model
            spacing: Screen.pixelDensity
            property var selected_constraint
            delegate: Rectangle {
                id: constraints_rectangle
                border.color: "black"
                border.width: Screen.pixelDensity*0.5
                width: constrains_list_view.width
                height: 2*type_text.implicitHeight+Screen.pixelDensity*5
                Component.onCompleted: {
                    var c = constrains_list_model.get(index).constraint
                    color = c.conflicting ? "red" : "white"
                    if (c.type === 9) {
                        visible = false
                        height = 0
                    }
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        listEntities.clear()
                        var c = constrains_list_model.get(index).constraint
                        if (selected_constraint != c){
                            selected_constraint = c
                            if(c.entityA!==null)
                                listEntities.append({"object":c.entityA})
                            if(c.entityB!==null)
                                listEntities.append({"object":c.entityB})
                            if(c.ptA!==null)
                                listEntities.append({"object":c.ptA})
                            if(c.ptB!==null)
                                listEntities.append({"object":c.ptB})
                        } else {
                            selected_constraint = null
                        }
                    }
                }
                Text{
                    id:type_text
                    text: "Constraint: " + constrains_list_model.get(index).constraint.objectName
                    font.pointSize: 12
                    anchors.top:parent.top
                    anchors.left: parent.left
                    anchors.right:delete_button.left
                    anchors.margins: 5
                }
                Text{
                    text: "Type: "+ constrains_list_model.get(index).constraint.type_string
                    font.pointSize: 12
                    anchors.bottom: parent.bottom
                    anchors.right:delete_button.left
                    anchors.left: parent.left
                    anchors.margins: 5

                }
                Button{
                    id:delete_button
                    anchors.top:parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    width: height
                    text:"\uf057"
                    anchors.margins: Screen.pixelDensity*0.5
                    font.family: "FontAwesome"
                    onClicked: {
                        constrains_list_model.get(index).constraint.kill()
                        constrains_list_model.remove(index)
                        sketch.constraints.apply(sketch)
                        sketch.store_state(sketch.undo_buffer.length+1);
                    }
                }
            }
        }
    }

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 5
        visible: switch_views
        onVisibleChanged: {



        }
        Rectangle {
            id: listRect
            color: "white"
            border.width: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: parent.height/2
            Layout.minimumHeight: Screen.pixelDensity*5
            ListView {
                anchors.fill: parent
                anchors.margins: 5
                clip: true
                model: listEntities
                delegate:
                    Item{
                    width: listRect.width
                    height: Screen.pixelDensity*7
                    Text {
                        anchors.margins: 5
                        height: parent.height
                        anchors.top:parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: unselect_button.left
                        text: object.class_type+ " " + object.objectName
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 10;
                    }
                    Button{
                        anchors.margins: 5
                        id:unselect_button
                        text:"\u00d7"
                        width: height
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        onClicked: {
                            listEntities.remove(index)
                        }
                    }
                }
            }
        }
        GridLayout{
            id: constraintButtons
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Screen.pixelDensity*5*2
            Layout.maximumHeight: parent.width
            columns: 3
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Button {
                    id: horz_const
                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    Text{
                        text: "\u2015"
                        font.family: "Code2000"
                        anchors.fill: parent
                        fontSizeMode:Text.Fit
                        font.pixelSize: parent.width
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }
            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Button {
                    id: vert_const
                    Text{
                        text: "\u007C"
                        font.family: "Code2000"
                        anchors.fill: parent
                        fontSizeMode:Text.Fit
                        font.pixelSize: parent.width
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }
            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Button {
                    id: equL_const
                    Text{
                        text: "\uFF1D"
                        font.family: "Code2000"
                        anchors.fill: parent
                        fontSizeMode:Text.Fit
                        font.pixelSize: parent.width
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }

            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Button {
                    id: para_const
                    Text{
                        text: "\u2225"
                        font.family: "Code2000"
                        anchors.fill: parent
                        font.pixelSize: parent.width
                        fontSizeMode:Text.Fit
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }

            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Button {
                    id: perp_const
                    Text{
                        text: "\u22A5"
                        font.family: "Code2000"
                        anchors.fill: parent
                        fontSizeMode:Text.Fit
                        font.pixelSize: parent.width
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }

            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible:false
                Button {
                    id: midP_const
                    Text{
                        text: "\u237F"
                        font.family: "Code2000"
                        anchors.fill: parent
                        fontSizeMode:Text.Fit
                        font.pixelSize: parent.width
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    anchors.centerIn: parent
                    width: height
                    height: Math.min(parent.width,parent.height)
                    checkable: true
                    checked: false
                    enabled: false
                    onEnabledChanged: this.checked = false
                }

            }
        }
        Item {
            id: leng_const
            property bool checked: leng_const_button.checked
            property double value: leng_const_value.text
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Screen.pixelDensity*5
            Layout.maximumHeight: Screen.pixelDensity*10
            enabled: false
            Button {
                id: leng_const_button
                Text{
                    text: "Length"
                    anchors.fill: parent
                    fontSizeMode:Text.Fit
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                height: parent.height
                checkable: true
                checked: false
                enabled: parent.enabled
                onEnabledChanged: this.checked = false
            }
            TextField {
                id: leng_const_value
                height: parent.height
                anchors.left: leng_const_button.right
                anchors.right: parent.right
                validator: RegExpValidator {
                    regExp: /^([0-9]*)\.([0-9]*)|([0-9]+)$/
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                placeholderText: "Length"
                text: "100.0"
                horizontalAlignment: TextInput.AlignRight
                enabled: parent.enabled
                onTextChanged: leng_const_button.checked = true
            }
        }
        Item {
            id: dist_const
            property bool checked: dist_const_button.checked
            property double value: dist_const_value.text
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Screen.pixelDensity*5
            Layout.maximumHeight: Screen.pixelDensity*10
            enabled: false
            Button {
                id: dist_const_button
                height: parent.height
                Text{
                    text: "Distance"
                    anchors.fill: parent
                    fontSizeMode:Text.Fit
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                checkable: true
                checked: false
                enabled: parent.enabled
                onEnabledChanged: this.checked = false
            }
            TextField {
                id: dist_const_value
                anchors.left: dist_const_button.right
                anchors.right: parent.right
                height: parent.height
                validator: RegExpValidator {
                    regExp: /^([0-9]*)\.([0-9]*)|([0-9]+)$/
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                placeholderText: "Distance"
                text: "100.0"
                horizontalAlignment: TextInput.AlignRight
                enabled: parent.enabled
                onTextChanged: dist_const_button.checked = true
            }
        }
        Item {
            id: angl_const
            property bool checked: angl_const_button.checked
            property var value: angl_const_value.text
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Screen.pixelDensity*5
            Layout.maximumHeight: Screen.pixelDensity*10
            enabled: false
            Button {
                id: angl_const_button
                Text{
                    text: "Angle"
                    anchors.fill: parent
                    fontSizeMode:Text.Fit
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                checkable: true
                height: parent.height
                checked: false
                enabled: parent.enabled
                onEnabledChanged: this.checked = false
            }
            TextField {
                id: angl_const_value
                anchors.left: angl_const_button.right
                anchors.right: parent.right
                height: parent.height
                validator: RegExpValidator {
                    regExp: /^([0-9]*)\.([0-9]*)|([0-9]+)$/
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                placeholderText: "Angle"
                text: "90"
                horizontalAlignment: TextInput.AlignRight
                enabled: parent.enabled
                onTextChanged: angl_const_button.checked = true
            }
        }
        Button {
            id: validate
            Text{
                text: "Validate constraints"
                anchors.fill: parent
                fontSizeMode:Text.Fit
                horizontalAlignment:Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: Screen.pixelDensity*5
            Layout.maximumHeight: Screen.pixelDensity*10
            onClicked:{
                sketch.constraints.add()
                listEntities.clear()
                horz_const.enabled = false
                vert_const.enabled = false
                leng_const.enabled = false
                equL_const.enabled = false
                dist_const.enabled = false
                para_const.enabled = false
                perp_const.enabled = false
                angl_const.enabled = false
                midP_const.enabled = false
                sketch.constraints.apply(sketch)
                sketch.store_state(sketch.undo_buffer.length+1);
            }
        }
    }

}
