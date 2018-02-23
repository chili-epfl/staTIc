import QtQuick 2.7
import QtQuick.Window 2.2
import QtMultimedia 5.6
import QtQuick.Controls 1.4
import ARToolkit 1.0
import QtQuick.Layouts 1.1
import IOBoardFile 1.0
Item {
    signal pageExit();
    visible: true
    id : root
    anchors.fill: parent
    property string file_path
    onFile_pathChanged: ioBoardFile.loadFile(file_path)
    IOBoardFile{
        id:ioBoardFile
        signal configReady();
        property variant positions:[];

        property real pxmmratio:max_dist>0 ? 0.75*root.width/max_dist : 1;
        property vector2d center
        property real max_dist:-1
        onLoadedChanged: {
            var cos_60=Math.cos(0.5235987756)
            var sin_60=Math.sin(0.5235987756)
            var center_x=0;
            var center_y=0
            for(var i = 0; i < ioBoardFile.tags_configuration.length; i++){
                center_x+=ioBoardFile.tags_configuration[i]["x"];
                center_y+=ioBoardFile.tags_configuration[i]["y"];
                var c=ioBoardFile.tags_configuration[i]["x"]/(90*cos_60)
                c= Math.round(c)
                var r= c%2==0 ? ioBoardFile.tags_configuration[i]["y"]/90 :
                                ioBoardFile.tags_configuration[i]["y"]/90-sin_60;
                r= Math.round(r)
                positions.push(Qt.vector2d(c,r));
                var dx = ioBoardFile.tags_configuration[i]["x"]
                var dy = ioBoardFile.tags_configuration[i]["y"];
                if(Math.sqrt((dx*dx)+(dy*dy))>max_dist){
                    max_dist=Math.sqrt((dx*dx)+(dy*dy));
                }
            }
            center=Qt.vector2d(center_x/ioBoardFile.tags_configuration.length,center_y/ioBoardFile.tags_configuration.length)
            max_dist=Math.max(300,max_dist);
            var _pxmmratio=max_dist>0 ? 0.75*root.width/max_dist : 1;
            var _tag_size_px= 25*_pxmmratio
            col_n= (Math.max(root.width,root.height)/_tag_size_px)
            configReady();
        }
    }

    signal update_from_UI(int origin_id);
    property int tag_size_px: 25*ioBoardFile.pxmmratio
    property int col_n: 0

    //********Editing Part************


    Item{
        id: grid_item
        width: 10
        height: 10
        x: root.width/2-ioBoardFile.center.x*ioBoardFile.pxmmratio
        y: ioBoardFile.center.y*ioBoardFile.pxmmratio+root.height/2

        Repeater{
            id:grid_repeater
            anchors.fill: parent
            model: col_n*col_n
            Image{
                source:"qrc:/ui/UI/BoardDesigner/hex.png"
                height: tag_size_px*90/35
                mipmap: true
                fillMode:Image.PreserveAspectFit

                property int model_index:index
                property int c: Math.floor(model_index/col_n) - Math.floor(col_n/2)
                property int r: (model_index%col_n) - Math.floor(col_n/2)
                x:Math.round(c*Math.cos(0.5235987756)*tag_size_px*90/35- width/2)
                y: c%2==0?
                       Math.round(-r*tag_size_px*90/35-height/2) :
                       Math.round((-r-Math.sin(0.5235987756))*tag_size_px*90/35- height/2)

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
                    onUpdate_from_UI:update_from_UI(origin_id)
                }
                Connections{
                    target: ioBoardFile
                    onConfigReady: update_from_image()
                }

                function update_from_image(){
                    var world_x= c*Math.cos(0.5235987756)*90
                    var world_y= c%2==0? 90*r : 90*(r+Math.sin(0.5235987756))
                    if(c==0 && r==0)
                        origin=true
                    var index=ioBoardFile.positions.indexOf(Qt.vector2d(c,r))
                    if(index>-1){
                        tag_name=ioBoardFile.tags_configuration[index]["id"]

                        tag_index=index;
                        orientation=ioBoardFile.tags_configuration[index]["rotation"]
                        current_global_orientation=0
                        return;
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
                    source: parent.tag_index==-1 ? "" :
                    tag_name.length==1 ?
                        "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/00"+parent.tag_name+".png"
                    : tag_name.length==2?
                        "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/0"+parent.tag_name+".png"
                    : "qrc:/ui/UI/BoardDesigner/AR_MATRIX_CODE_4x4_BCH_13_9_3/"+tag_name+".png"
                    width: tag_size_px
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
                    width: tag_size_px
                    anchors.centerIn: parent
                    fillMode:Image.PreserveAspectFit
                    mipmap: true
                }
                MouseArea{
                    width: tag_size_px
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
        Row{
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 20
            spacing: 10
            Button{
                text:"Save(*)"
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
                        pageExit()
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
        width: parent.width/3
        onUpdate_Model: root.update_from_UI(origin_id)
    }

}
