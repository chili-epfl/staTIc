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
                            Rectangle{
                                color:"white"
                                anchors.fill: parent

                                GridLayout{
                                    rows: 5
                                    columns: 2
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    Label {
                                        text: "Name"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id:name_field
                                        textColor: "#2f3439"
                                        placeholderText: "Enter Name"
                                        font.pointSize:12
                                        text:name
                                        onTextChanged: name=text;

                                    }

                                    Label {
                                        text: "Density(g/cm3)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: density_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        validator: DoubleValidator {bottom: 0.1}
                                        text: density
                                        font.pointSize: 12
                                        onTextChanged: density=text;

                                    }


                                    Label {
                                        text: "Price(Chf)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: price_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        validator: DoubleValidator {bottom: 0}
                                        text: price
                                        font.pointSize: 12
                                        onTextChanged: price=Number.fromLocaleString(Qt.locale(),text);
                                    }


                                    Label {
                                        text: "Elasticity(N/mm2)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: young_field
                                        textColor: "#2f3439"
                                        validator: DoubleValidator {bottom: 0.1}
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: young
                                        font.pointSize: 12
                                        onTextChanged: young=text;

                                    }
                                    Label {
                                        text: "Shear Modulus(N/mm2)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: g_field
                                        textColor: "#2f3439"
                                        validator: DoubleValidator {bottom: 0.1}
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text:g
                                        font.pointSize: 12
                                        onTextChanged: g=text;

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
                                        color: "#2f3439"
                                    }
                                    TextField {
                                        id: fmk_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: fmk
                                        font.pointSize: 12
                                        onTextChanged: fmk=text;
                                        validator: DoubleValidator {bottom: 0.1}


                                    }
                                    Label {
                                        text: "Fvd(N/mm2)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: fvk_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: fvk
                                        font.pointSize: 12
                                        onTextChanged: fvk=text;
                                        validator: DoubleValidator {bottom: 0.1}


                                    }
                                    Label {
                                        text: "Ft0d(N/mm2)"
                                        color: "#2f3439"
                                    }
                                    TextField {
                                        id: ft0_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: ft0

                                        font.pointSize: 12
                                        onTextChanged: ft0=text;
                                        validator: DoubleValidator {bottom: 0.1}


                                    }
                                    Label {
                                        text: "Fc0d(N/mm2)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: fc0_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: fc0
                                        font.pointSize: 12
                                        onTextChanged: fc0=text;
                                        validator: DoubleValidator {bottom: 0.1}


                                    }

                                    Label {
                                        text: "Ft90d(N/mm2)"
                                        color: "#2f3439"

                                    }
                                    TextField {
                                        id: ft90_field
                                        text: ft90
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        font.pointSize: 12
                                        onTextChanged: ft90=text;
                                        validator: DoubleValidator {bottom: 0.1}


                                    }
                                    Label {
                                        text: "Fc90d(N/mm2)"
                                        color: "#2f3439"
                                    }
                                    TextField {
                                        id: fc90_field
                                        textColor: "#2f3439"
                                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        text: fc90
                                        font.pointSize: 12
                                        onTextChanged: fc90=text;
                                        validator: DoubleValidator {bottom: 0.1}

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
                            text:"Clear"
                            onClicked: {
                                name="";
                                density=1;
                                price=1;
                                young=7000;
                                g=440;
                                fmk=10;
                                fvk=10;
                                ft0=10;
                                fc0=10;
                                fc90=10;
                                ft90=10;

                            }
                        }
                        Button{
                            text:"Open"
                            onClicked: file_dialog.visible=true
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

            Item{
                id:file_dialog
                visible: false
                anchors.fill: parent
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        parent.visible=false
                    }
                }
                Rectangle{
                    width: 2/3*parent.width
                    height: 2/3*parent.height
                    color: "#2f3439"
                    border.color: "#F0F0F0"
                    border.width: 10
                    anchors.centerIn: parent
                    MouseArea{
                        anchors.fill: parent
                        propagateComposedEvents: false
                    }
                    ColumnLayout{
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 10
                        Label{
                            Layout.fillWidth: true
                            Layout.preferredHeight: implicitHeight
                            text: "Press long to delete"
                            color: "#F0F0F0"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                        ListView{
                            id:file_listview
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: materialManager
                            clip: true
                            delegate: Rectangle{
                                width: file_listview.width;
                                height: file_name_text.implicitHeight*2
                                color: "#F0F0F0"
                                MouseArea{
                                    anchors.fill: parent
                                    onDoubleClicked: {
                                        name=materialManager.get(index,"name");
                                        density=materialManager.get(index,"density");
                                        price=materialManager.get(index,"price");
                                        young=materialManager.get(index,"young");
                                        g=materialManager.get(index,"g");
                                        fmk=materialManager.get(index,"fmk");
                                        fvk=materialManager.get(index,"fvk");
                                        ft0=materialManager.get(index,"ft0");
                                        fc0=materialManager.get(index,"fc0");
                                        fc90=materialManager.get(index,"fc90");
                                        ft90=materialManager.get(index,"ft90");
                                        currentImage=materialManager.get(index,"image");
                                        file_dialog.visible=false;
                                    }
                                    onPressAndHold: {
                                        delete_button.visible=true;
                                    }
                                }
                                Text{
                                    id:file_name_text
                                    width: parent.width
                                    text:  display
                                    color: "#2f3439"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Button{
                                    id:delete_button
                                    anchors.centerIn:parent
                                    visible: false
                                    text:"Delete"
                                    onClicked: {
                                        materialManager.deleteMaterial(index)
                                    }
                                    Timer{
                                        interval: 2000
                                        running: parent.visible
                                        repeat: false
                                        onTriggered: parent.visible=false
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




