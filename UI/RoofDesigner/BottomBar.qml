import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Window 2.0

ToolBar {
    property var availableUnits: [
        "mm",
        "cm",
        "dm",
        "m",
        "dam",
        "hm",
        "km"
    ]

    property real scaleFactor: sketch.scaleFactor

    property real ruler_length : 1
    property int  ruler_order: 0

    onScaleFactorChanged: {
        var ratio = scaleFactor / Screen.pixelDensity
        var order = 1 - Math.floor(Math.log(ratio) / Math.LN10)
        if(order <= 0) {
            ruler_order = 0
        }
        else if(order > 6) {
            ruler_order = 6
        }
        else {
            ruler_order = order;
        }

        ruler_length = scaleFactor * Math.pow(10, order)

    }

    Rectangle{
        width: sketch.zoomFactor*ruler_length
        height: Screen.pixelDensity
        color: "black"
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: Screen.pixelDensity*15
        Rectangle{
            width: Screen.pixelDensity
            height: Screen.pixelDensity*3
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            color:parent.color
        }
        Rectangle{
            width: Screen.pixelDensity
            height: Screen.pixelDensity*3
            anchors.left: parent.right
            anchors.verticalCenter: parent.verticalCenter
            color:parent.color
        }
        Text{
            anchors.bottom:parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text:"1 "+ availableUnits[ruler_order]
            anchors.margins: 10
        }
    }


}

