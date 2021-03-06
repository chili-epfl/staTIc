import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Window 2.0
import JSONSketch 1.0

Page {
    id: sketchScreen

    JSONSketch {
        id: json_sketch
    }

    property string sketch_to_load

    Component.onCompleted: {
        if(sketch_to_load.length>0){
            json_sketch.loadSketch(scenariosPath,sketch_to_load+ ".json", sketch)
            topBar.sketch_name=sketch_to_load;
            sketch.store_state(sketch.undo_buffer.length+1);
        }
    }

    property alias aux_loader: aux_loader

    property alias constraintsPanel: constraintsPanel

    property bool isBackgroundSet: (background_picture_url!="")
    property alias visibleBackground: backgroundImage.visible
    property alias background_picture_url: sketch.background_picture_url

    property alias visibleGrid: gridCanvas.visible

    property string toolName: "SelectTool"
    property var current_tool: toolName == "SelectTool" ? select_tool :
                                                          toolName == "InsertTool" ? insert_tool :
                                                                                     toolName == "MoveTool"? move_tool : delete_tool

    property alias sketch: sketch

    header: TopBar{
        id:topBar
        height: Screen.pixelDensity*5*scalePixelFactor
        visible: aux_loader.status!=Loader.Ready
    }

    footer: BottomBar{
        height: Screen.pixelDensity*5*scalePixelFactor
        visible: aux_loader.status!=Loader.Ready
    }

    Image {
        id: backgroundImage
        mipmap: true
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: background_picture_url
        transform:  Scale{
            origin.x:sketch.zoom_origin_x
            origin.y:sketch.zoom_origin_y
            xScale: sketch.zoomFactor
            yScale: sketch.zoomFactor
        }

    }


    Canvas {
        id:gridCanvas
        anchors.fill: parent
        property real cellSize: Screen.pixelDensity*10
        Connections{
            ignoreUnknownSignals:true
            target: sketch
            onZoomFactorChanged:gridCanvas.requestPaint()
        }
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset()
            ctx.translate(-sketch.zoom_origin_x*(sketch.zoomFactor%1),
                          -sketch.zoom_origin_y*(sketch.zoomFactor%1) );
            ctx.scale(1+sketch.zoomFactor%1, 1+sketch.zoomFactor%1);
            for(var i=0; i<width;i=i+cellSize )
                for(var j=0; j<height;j=j+cellSize )
                ctx.strokeRect(i, j, cellSize, cellSize);


        }

    }
    Sketch{
        id:sketch
//        mouse_area.enabled: Qt.platform.os=="android"? current_tool!==select_tool : true
//        pinch_area.enabled: Qt.platform.os=="android" ? current_tool===select_tool : false

    }



    ConstraintsPanel {
        id: constraintsPanel
        visible: current_tool===select_tool
        x:parent.width-(width+10)
        y:parent.height/2-height/2
        height: parent.height-10
        width: Screen.pixelDensity*50
    }

    ToolsMenu {
        x:10
        y:parent.height/2-height/2
    }

    Loader{
        id:aux_loader
        anchors.fill: parent

    }

    SelectTool{
        id:select_tool
    }

    InsertTool{
        id:insert_tool
    }
    MoveTool{
        id:move_tool
    }
    DeleteTool{
        id:delete_tool
    }
}
