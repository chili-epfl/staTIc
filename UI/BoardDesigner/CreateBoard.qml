import QtQuick 2.7
import QtQuick.Window 2.2
import QtMultimedia 5.6
import QtQuick.Controls 1.4
import ARToolkit 1.0
import QtQuick.Layouts 1.1
import IOBoardFile 1.0
Item {
    visible: true
    id : root
    anchors.fill: parent

    property real scaleX:video_output.contentRect.width/filter.cameraResolution.width
    property real scaleY:video_output.contentRect.height/filter.cameraResolution.height

    property int tag_size_px: 10*Screen.pixelDensity
    property int col_n: 0
    property real grid_rotation_in_image: 0

    signal update_from_image();
    signal update_from_UI(int origin_id);
    state: "capturing"
    states:[
        State {
            name: "capturing"
            PropertyChanges {
                target: captureButton
                text: "Capture"
            }
            StateChangeScript{
                script: {
                    camera.start()
                }
            }
            PropertyChanges {
                target: video_output
                visible:true
            }
            PropertyChanges {
                target: grid_item
                visible:false
            }
        },
        State{
            name:"previewing"
            PropertyChanges {
                target: captureButton
                text: "Capture Again"
            }
            StateChangeScript{
                script: {
                    //camera.stop()
                }
            }
            PropertyChanges {
                target: video_output
                visible:false
            }
            PropertyChanges {
                target: grid_item
                visible:false
            }
        },
        State{
            name:"editing"

            PropertyChanges {
                target: video_output
                visible:false
            }
            PropertyChanges {
                target: captureButton
                visible: false
            }
            PropertyChanges {
                target: canvas
                visible:false
            }
            PropertyChanges {
                target: grid_item
                visible:true
            }
            StateChangeScript{
                script: {
                    camera.stop()
                    update_from_image()
                }
            }
        }
    ]

    IOBoardFile{
        id:ioBoardFile
    }

    Image{
        id:background
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent
    }

    Camera {
        id: camera
        deviceId: Qt.platform.os=="android" ?QtMultimedia.availableCameras[0].deviceId :QtMultimedia.availableCameras[1].deviceId
        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceAuto
        focus.focusMode: CameraFocus.FocusContinuous
        exposure.exposureMode: CameraExposure.ExposureModeVendor
        exposure.meteringMode: CameraExposure.MeteringMatrix
        imageProcessing.colorFilter: CameraImageProcessing.ColorFilterVendor
        imageProcessing.denoisingLevel: 1
        imageProcessing.sharpeningLevel: 1
        viewfinder.resolution: "640x480"
        captureMode: Camera.CaptureStillImage

        imageCapture {
            onImageSaved: {
                root.state="previewing"
                background.source="file://"+path
                camera.stop()
            }
        }

    }

    VideoOutput {
        id:video_output
        source: camera
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop
        focus : visible // to receive focus and capture key events when visible
        filters: [filter]
    }

    Canvas {
        id: canvas
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        onPaint: {
            var ctx = canvas.getContext('2d')
            ctx.reset();
            var max = filter.detectedMarkers.length
            ctx.font="20pt sans-serif"
            ctx.textBaseline="middle"
            for (var i = 0; i< max; ++i){
                ctx.fillStyle = "red";
                var centreX = video_output.contentRect.x+0.5*scaleX*(filter.detectedMarkers[i]["TLCorner"].x+filter.detectedMarkers[i]["BRCorner"].x);
                var centreY = video_output.contentRect.y+0.5*scaleY*(filter.detectedMarkers[i]["TLCorner"].y+filter.detectedMarkers[i]["BRCorner"].y);
                ctx.fillText("o",centreX-5,centreY);
            }
        }
    }
    ARToolkit{
        id:filter
        matrixCode: ARToolkit.MATRIX_CODE_4x4_BCH_13_9_3
        projectionMatrix:Qt.matrix4x4(
                             1.4272479930179818e+03 ,0 ,6.4750000000000000e+02,0,
                             0, 1.4272479930179818e+03 , 4.8550000000000000e+02,0,
                             0,0,1,0,
                             0,0,0,1)
        onDetectedMarkersChanged: {
            if(detectedMarkers.length>0){
                canvas.requestPaint()
            }
        }
        flip_image: Qt.platform.os=="android"

    }

    //********Editing Part************
    Item{
        opacity: slider_alpha.value
        id: grid_item
        rotation: -grid_rotation_in_image*180/Math.PI //anticlockwise
        width: 10
        height: 10
        property vector2d video_output_size
        property vector2d scale_factor
        property vector2d original_root_size: Qt.vector2d(root.width,root.height)
        transformOrigin: Item.TopLeft
        x: filter.detectedMarkers.length>0 ? (root.width/original_root_size.x)*(video_output_size.x+0.5*scale_factor.x*(filter.detectedMarkers[0]["TLCorner"].x+filter.detectedMarkers[0]["BRCorner"].x)) : root.width/2
        y: filter.detectedMarkers.length>0 ? (root.height/original_root_size.y)*(video_output_size.y+0.5*scale_factor.y*(filter.detectedMarkers[0]["TLCorner"].y+filter.detectedMarkers[0]["BRCorner"].y)) : root.height/2

        Repeater{
            id:grid_repeater
            anchors.fill: parent
            model: col_n*col_n
            Image{
                source:"qrc:/ui/UI/BoardDesigner/hex.png"
                height: (root.height/grid_item.original_root_size.y)*tag_size_px*90/35
                mipmap: true
                fillMode:Image.PreserveAspectFit

                property int model_index:index
                property int c: Math.floor(model_index/col_n) - Math.floor(col_n/2)
                property int r: (model_index%col_n) - Math.floor(col_n/2)
                x:Math.round(c*Math.cos(0.5235987756)*(root.width/grid_item.original_root_size.x)*tag_size_px*90/35- width/2)
                y:c%2==0? Math.round(-r*(root.height/grid_item.original_root_size.y)*tag_size_px*90/35-height/2) : Math.round((-r-Math.sin(0.5235987756))*(root.height/grid_item.original_root_size.y)*tag_size_px*90/35- height/2)

                property int tag_index: -1
                property string tag_name
                property int orientation : 0
                property bool origin: false
                property int current_global_orientation:0
                property int current_origin_index:(col_n+1)*Math.floor(col_n/2)

                property real world_x: c*Math.cos(0.5235987756)*90
                property real world_y: c%2==0 ? r*90 : (r+Math.sin(0.5235987756))*90

                Connections{
                    target: root
                    onUpdate_from_image: update_from_image()
                    onUpdate_from_UI:update_from_UI(origin_id)
                }

                function update_from_image(){
                    var global_pos=mapToItem(root,width/2,height/2);
                    for(var i = 0; i < filter.detectedMarkers.length; i++){//if we can assume a minimum distance then replace i++ with i+=4
                        var dx = global_pos.x - (grid_item.video_output_size.x+0.5*grid_item.scale_factor.x*(filter.detectedMarkers[i]["TLCorner"].x+filter.detectedMarkers[i]["BRCorner"].x))
                        var dy = global_pos.y - (grid_item.video_output_size.y+0.5*grid_item.scale_factor.y*(filter.detectedMarkers[i]["TLCorner"].y+filter.detectedMarkers[i]["BRCorner"].y));
                        var distance=Math.sqrt((dx*dx)+(dy*dy))
                        if(distance < 10){
                            if(i==0)
                                origin=true
                            tag_name=filter.detectedMarkers[i]["id"].substr(4)

                            tag_index=i;
                            orientation=(180/Math.PI)*(Math.atan2(
                                        (-filter.detectedMarkers[i]["TRCorner"].y+filter.detectedMarkers[i]["TLCorner"].y),
                                        (filter.detectedMarkers[i]["TRCorner"].x-filter.detectedMarkers[i]["TLCorner"].x))-grid_rotation_in_image)
                            orientation=(Math.round(orientation/60)*60)
                            orientation += (orientation>180) ? -360 : (orientation<-180) ? 360 : 0
                            current_global_orientation=0
                            return;
                        }
                    }
                    origin=false
                    tag_name=""
                    tag_index=-1;
                    orientation=0;
                    current_global_orientation=0
                }

                function update_from_UI(origin_id){
                    orientation=(orientation-(side_box.global_orientation-current_global_orientation))
                    orientation += (orientation>180) ? -360 : (orientation<-180) ? 360 : 0
                    current_global_orientation=side_box.global_orientation
                    var world_x_origin=0;
                    var world_y_origin=0;

                    if(origin_id==-1) origin_id=current_origin_index

                    if(index==origin_id)
                        origin=true
                    else
                        origin=false

                    var c_origin= Math.floor(origin_id /col_n) - Math.floor(col_n/2)
                    var r_origin= (origin_id % col_n) - Math.floor(col_n/2)
                    world_x_origin= c_origin*Math.cos(0.5235987756)*90
                    world_y_origin= c_origin%2==0? r_origin*90: (r_origin+Math.sin(0.5235987756))*90

                    var pre_rotation_world_x=c*Math.cos(0.5235987756)*90 - world_x_origin
                    var pre_rotation_world_y= c%2==0? 90*r - world_y_origin: 90*(r+Math.sin(0.5235987756)) - world_y_origin

                    var current_global_orientation_rad=Math.PI*current_global_orientation/180

                    world_x=Math.cos(-current_global_orientation_rad)*pre_rotation_world_x-Math.sin(-current_global_orientation_rad)*pre_rotation_world_y
                    world_y=Math.sin(-current_global_orientation_rad)*pre_rotation_world_x+Math.cos(-current_global_orientation_rad)*pre_rotation_world_y

                    current_origin_index=origin_id
                }

                Image{
                    source:{
                        var name=""
                        if(parent.tag_index>-1){
                            if(parent.tag_name.length==1)
                                name= "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/00"+parent.tag_name+".png"
                            else if(parent.tag_name.length==2)
                                name= "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/0"+parent.tag_name+".png"
                            else
                                name= "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/"+parent.tag_name+".png"
                        }
                        return name;
                    }
                    width: (root.width/grid_item.original_root_size.x)*tag_size_px
                    anchors.centerIn: parent
                    fillMode:Image.PreserveAspectFit
                    mipmap: true
                    rotation: -parent.orientation-parent.current_global_orientation

                }

                Image{
                    id:xy_coordinate
                    visible:parent.origin | side_box.show_xy
                    rotation: -side_box.global_orientation//anticlockwise
                    source:"qrc:/ui/UI/BoardDesigner/xy.png"
                    width: (root.width/grid_item.original_root_size.x)*tag_size_px
                    anchors.centerIn: parent
                    fillMode:Image.PreserveAspectFit
                    mipmap: true
                }
                MouseArea{
                    width: (root.width/grid_item.original_root_size.x)*tag_size_px
                    height:width
                    anchors.centerIn: parent
                    onClicked:{
                        side_box.current_id= parent.model_index
                    }
                }
            }
        }
    }

    //ToolBar
    Rectangle{
        width: parent.width
        height: Screen.pixelDensity*10
        color:"#2f3439"
        anchors.bottom: parent.bottom
        Slider{
            id:slider_alpha
            visible: root.state=="editing"
            x: 0
            orientation: Qt.Horizontal
            anchors.verticalCenter: parent.verticalCenter
            stepSize: 0.5
            minimumValue: 0
            maximumValue: 1
            value: visible? 1 : 0
            NumberAnimation on value{
            }
            tickmarksEnabled: true
            width: parent.width/4
            Component.onCompleted: value=1
            Label{
                text:"Opacity"
                color: "white"
                anchors.bottom: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Row{
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 20
            spacing: 10
            Button{
                id : captureButton
                text : "Capture Frame"
                property string filename: "save.jpg"
                onClicked: {
                    if(root.state=="capturing"){
                        camera.imageCapture.captureToLocation(filename)
                    }
                    else if(root.state=="previewing")
                        root.state="capturing"
                }
            }
            Button{
                text:"Edit"
                visible: root.state=="previewing"
                onClicked: {
                    if(filter.detectedMarkers.length>0){
                        grid_rotation_in_image=Math.atan2(
                                    (-filter.detectedMarkers[0]["TRCorner"].y+filter.detectedMarkers[0]["TLCorner"].y),
                                    (filter.detectedMarkers[0]["TRCorner"].x-filter.detectedMarkers[0]["TLCorner"].x))
                        tag_size_px=Math.sqrt(
                                    Math.pow(scaleY*(filter.detectedMarkers[0]["TLCorner"].y-filter.detectedMarkers[0]["TRCorner"].y),2)+
                                    Math.pow(scaleX*(filter.detectedMarkers[0]["TLCorner"].x-filter.detectedMarkers[0]["TRCorner"].x),2))
                        grid_item.video_output_size=Qt.vector2d(video_output.contentRect.x,video_output.contentRect.y)
                        grid_item.scale_factor=Qt.vector2d(scaleX,scaleY)
                    }
                    grid_item.original_root_size=Qt.vector2d(root.width,root.height)
                    col_n=(Math.max(root.width,root.height)/tag_size_px)
                    root.state="editing"
                }
            }
            Button{
                text:"Save(*)"
                visible: root.state=="editing"
                onClicked:{
                    var conf=[]
                    for(var i = 0; i < grid_repeater.count; i++){
                        if(grid_repeater.itemAt(i).tag_index>=0){
                            var item={}
                            item["id"]=grid_repeater.itemAt(i).tag_name;
                            item["rotation"]=grid_repeater.itemAt(i).orientation;
                            item["x"]=grid_repeater.itemAt(i).world_x;
                            item["y"]=grid_repeater.itemAt(i).world_y;
                            conf.push(item);
                        }
                    }
                    ioBoardFile.tags_configuration=conf;
                    if(ioBoardFile.writeFile()){
                        text="Save"
                    }
                    else{
                        text="Save(*)"
                    }
                }
            }
        }
    }

    SideBox{
        id:side_box
        visible: root.state=="editing"
        width: parent.width/3
        onUpdate_Model: root.update_from_UI(origin_id)
    }

}
