import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtMultimedia 5.5
import MaterialsManager 1.0
Rectangle {
    signal pageExit();
    visible: true
    radius: 5
    border.color: "#F0F0F0"
    border.width: 10
    color: "#2f3439"

    anchors.fill: parent
    anchors.centerIn: parent

    property url currentImage;
    property string name;
    property real density:1;
    property real price:1;
    property real young:7000;
    property real g:440;
    property real fmk:10;
    property real fvk:10;
    property real ft0:10;
    property real fc0:10;
    property real fc90:10;
    property real ft90:10;

    MaterialsManager{
        id:materialManager

    }

    Rectangle{
        id: root
        radius: 5
        border.color: "#F0F0F0"
        border.width: 10
        color: "transparent"
        anchors.fill:parent
        anchors.margins: 50
        Rectangle{
            anchors.fill: parent
            color: "#2f3439"
            Image {
                anchors.fill: parent
                source: "qrc:/ui/UI/MaterialDesigner/assets/backgroud.jpg"
            }
            Image{
                id:texture_image
                height: width
                width: parent.width/2-40
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.horizontalCenter
                anchors.margins: 10
                source:  currentImage!=""? currentImage: "qrc:/ui/UI/MaterialDesigner/assets/image_placeholder.png"
                fillMode: Image.PreserveAspectCrop
                Rectangle{
                    anchors.fill: parent
                    color: currentImage!=""? "#00E0E0E0":"#AAE0E0E0"
                    Column{
                        anchors.fill: parent
                        Item{
                            width: parent.width
                            height: parent.height/2
                            Image {
                                id: take_picture;
                                width: Math.min(parent.width/3,parent.height)
                                height: width
                                anchors.centerIn: parent
                                source: currentImage!=""?"qrc:/ui/UI/MaterialDesigner/assets/camera_icon_trasparent.png" :"qrc:/ui/UI/MaterialDesigner/assets/camera_icon.png"
                                MouseArea{
                                    anchors.fill: parent;
                                    onClicked:{
                                        loader.asynchronous=true;
                                        loader.source="qrc:/ui/UI/MaterialDesigner/TakePicture.qml"
                                    }
                                }
                            }
                        }
                        Item{
                            width: parent.width
                            height: parent.height/2
                            Image{
                                id:take_from_file
                                width: take_picture.width
                                height: width
                                anchors.centerIn: parent
                                source: currentImage!=""? "qrc:/ui/UI/MaterialDesigner/assets/sdcard_transparent.png":"qrc:/ui/UI/MaterialDesigner/assets/sdcard.png"
                                MouseArea{
                                    anchors.fill: parent;
                                    onClicked: {
                                        loader.asynchronous=false;
                                        loader.source="qrc:/ui/UI/MaterialDesigner/TakeFromFile.qml"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Item{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.horizontalCenter
                anchors.margins: 10
                width: parent.width/2 - 10
                height: parent.height
                ColumnLayout{
                    anchors.fill: parent
                    anchors.centerIn: parent
                    anchors.margins: 10
                    spacing: 10
                    TabView{
                        Layout.fillWidth:true
                        Layout.fillHeight:true
                        Tab{
                            title: "General"
                            anchors.fill: parent
                            Item{
                                //color:"red"
                                anchors.fill: parent

                                GridLayout{
                                    rows: 5
                                    columns: 2
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    Label {
                                        text: "Name"
                                    }
                                    TextField {
                                        id:name_field
                                        placeholderText: "Enter Name"
                                        font.pointSize:12
                                        onTextChanged: name=text;

                                    }

                                    Label {
                                        text: "Density(g/cm3)"
                                    }
                                    SpinBox {
                                        id: density_field
                                        decimals: 1
                                        stepSize: 0.1
                                        value: density
                                        minimumValue: 0.1
                                        maximumValue: 2
                                        font.pointSize: 12
                                        onValueChanged: density=value;

                                    }


                                    Label {
                                        text: "Price(Chf)"
                                    }
                                    SpinBox {
                                        id: price_field
                                        stepSize: 1
                                        value: price
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: price=value;

                                    }


                                    Label {
                                        text: "Elasticity(N/mm2)"
                                    }
                                    SpinBox {
                                        id: young_field
                                        stepSize: 500
                                        value: young
                                        minimumValue: 3000
                                        maximumValue: 30000
                                        font.pointSize: 12
                                        onValueChanged: young=value;

                                    }
                                    Label {
                                        text: "Shear Modulus(N/mm2)"
                                    }
                                    SpinBox {
                                        id: g_field
                                        value: g
                                        stepSize: 50
                                        minimumValue: 100
                                        maximumValue: 1500
                                        font.pointSize: 12
                                        onValueChanged: g=value;

                                    }

                                }
                            }
                        }

                        Tab{
                            title: "Resistance"
                            anchors.fill: parent
                            Rectangle{
                                color:"white"
                                anchors.fill: parent
                                GridLayout{
                                    rows: 6
                                    columns: 2
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    Label {
                                        text: "Fmd(N/mm2)"
                                    }
                                    SpinBox {
                                        id: fmk_field
                                        decimals: 1
                                        stepSize: 1
                                        value: fmk
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: fmk=value;

                                    }
                                    Label {
                                        text: "Fvd(N/mm2)"
                                    }
                                    SpinBox {
                                        id: fvk_field
                                        decimals: 1
                                        stepSize: 1
                                        value: fvk
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: fvk=value;

                                    }
                                    Label {
                                        text: "Ft0d(N/mm2)"
                                    }
                                    SpinBox {
                                        id: ft0_field
                                        decimals: 1
                                        stepSize: 1
                                        value: ft0
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: ft0=value;

                                    }
                                    Label {
                                        text: "Fc0d(N/mm2)"
                                    }
                                    SpinBox {
                                        id: fc0_field
                                        decimals: 1
                                        stepSize: 1
                                        value: fc0
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: fc0=value;

                                    }

                                    Label {
                                        text: "Ft90d(N/mm2)"
                                    }
                                    SpinBox {
                                        id: ft90_field
                                        decimals: 1
                                        stepSize: 1
                                        value: ft90
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: ft90=value;

                                    }
                                    Label {
                                        text: "Fc90d(N/mm2)"
                                    }
                                    SpinBox {
                                        id: fc90_field
                                        decimals: 1
                                        stepSize: 1
                                        value: fc90
                                        minimumValue: 1
                                        maximumValue: 100
                                        font.pointSize: 12
                                        onValueChanged: fc90=value;
                                    }

                                }
                            }
                        }

                    }

                    Row{
                        id:button_row
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 20
                        Button{
                            text: "Create"
                            onClicked: {
                                materialManager.createFile(texture_image.source,name,
                                                    density,price,
                                                    young,g,
                                                    fc0,fc90,
                                                    fmk,ft0,
                                                    ft90,fvk)
                            }
                        }
                        Button{
                            text: "Done"
                            onClicked: {
                               pageExit()
                            }
                        }
                    }
                }

            }

            Rectangle{
                color:"#2f3439"
                anchors.fill: parent
                visible:loader.status==Loader.Loading || loader.status==Loader.Ready
                Loader{
                    anchors.fill: parent
                    id:loader
                    focus:true
                    Connections{
                        target:loader.item
                        onExit:{
                            loader.source=""
                        }
                    }
                }
            }
        }


//        Rectangle{
//            id:messageBox
//            visible:false;
//            width: parent.width/3
//            height: parent.height/3
//            anchors.centerIn: parent
//            Text {
//                anchors.centerIn: parent
//                id: message_text
//            }
//            function message(text){
//                visible=true;
//                message_text.text=text;
//            }
//            Timer{
//                running: parent.visible
//                onTriggered: parent.visible=false;
//                interval: 5000
//            }
//        }
    }



}




