pragma Singleton
import QtQuick 2.7

QtObject {
    readonly property real appWidth: 950
    readonly property real appHeight: 700

    readonly property string defaultTool : "InsertTool"

    readonly property string exportPath: ""

    readonly property string assetsExportPath: "./"
    readonly property string backgroundImagePath: assetsExportPath + "background.jpg";

    readonly property bool displayIdentifier: false;

    readonly property color selectedColor: "blue"
    readonly property color labelColor: "#222222"

    readonly property int backgroundFillMode: Image.PreserveAspectCrop

    readonly property real pointSize: 50 // diameter

    readonly property real minimalPointDistance: 100
    readonly property real constructionLineWidth: 50 // diameter

    readonly property string selectedToolColor: "#444444"
    readonly property string toolColor: "black"

    readonly property color palette: "#40404040"
    readonly property color paletteHighlight: "#20000000"

    readonly property color captureImagePanelBackground: "black"

    readonly property color lineColor: "gray";
    readonly property real lineOpacity: 0.8;

    readonly property color topBarLabelColor: labelColor;

    readonly property color insertLineIsInsertable : selectedColor
    readonly property color insertLineIsNotInsertable : "red"

    readonly property color intermediatePointColor : "red"
    readonly property color insertPointSelected: selectedColor

    readonly property color roundedButtonLabelColor: labelColor;
    readonly property color roundedButtonBackground: "#42000000"
    readonly property color roundedButtonSelectedBackground: "#22000000"

    readonly property real contextMenuRadius: 5
    readonly property real contextMenuZ: 200
    readonly property color contextMenuColor:  Qt.rgba(palette.r, palette.g, palette.b, 1)

    readonly property string defaultCaptureImagePath: "pictures/background_default.jpg"
    property bool backgroundGridEnable: true
    property bool helpTipEnable: true
    property bool rulerEnable: true

    property bool canUndo: false
    property bool canRedo: false
}
