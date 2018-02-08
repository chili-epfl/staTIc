import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Rectangle {
    id: toolsMenu
    width:Screen.pixelDensity*10
    height: tool_list.model.count * Screen.pixelDensity*10

    Drag.active: drag_area.drag.active

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
        anchors.left: parent.right
        radius: width/2
        MouseArea{
            id:drag_area
            anchors.fill: parent
            drag.target: toolsMenu
            drag.onActiveChanged: {
                if(!drag.active){
                    var x_anchor=toolsMenu.x + toolsMenu.width + width
                    var y_anchor=toolsMenu.y + 0.5 * toolsMenu.height
                    if(x_anchor<Screen.pixelDensity*10 ||
                            x_anchor>toolsMenu.parent.width-Screen.pixelDensity*10 ||
                            y_anchor<Screen.pixelDensity*10 ||
                            y_anchor>toolsMenu.parent.height-Screen.pixelDensity*10
                            ){
                        toolsMenu.x=-toolsMenu.width
                        toolsMenu.y=toolsMenu.parent.height/2-toolsMenu.height/2
                    }
                }

            }
        }
    }
    ListView {
        id:tool_list
        anchors.fill: parent
        clip:true
        currentIndex:0

        model:ListModel{
            ListElement {
                name: "SelectTool"
                icon: "\uf245"
            }
            ListElement {
                name: "InsertTool"
                icon: "\uf040"
            }
            ListElement {
                name: "MoveTool"
                icon: "\uf047"
            }
            ListElement {
                name: "DeleteTool"
                icon: "\uf00d"
            }
        }
        delegate: Rectangle {
            width: Screen.pixelDensity*10
            height: width
            color: ListView.isCurrentItem ? "#40404040": "#20000000"
            Label {
                text: icon
                font.family: "FontAwesome"
                font.pointSize: 24
                color: ListView.isCurrentItem ? "#444444" : "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    sketchScreen.toolName=name
                    tool_list.currentIndex=index
                }
            }
        }
    }

}



