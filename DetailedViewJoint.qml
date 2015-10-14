import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

Rectangle{
    property alias model: listView.model
    property alias count: listView.count
    id:outerRect
    anchors.fill: parent
    anchors.margins: 2
    radius: 1
    color: "transparent"
    ListView{
        Component{
            id:delegate
            Rectangle{
                color: "#FFFFFF99"
                width: outerRect.width; height:0.3*width
                border.width: 1
                radius: 1
                RowLayout{
                    anchors.fill: parent
                    Rectangle{
                        color: "transparent";
                        Layout.preferredHeight:  parent.height
                        Layout.preferredWidth:   0.3*parent.width
                        Image {
                            anchors.margins: 1
                            fillMode: Image.PreserveAspectFit
                            //source: icon;
                            anchors.fill: parent
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ListView.currentIndex = index
                            }
                        }
                    }
                    Rectangle{
                        Layout.preferredHeight:  parent.height - 5
                        Layout.preferredWidth:   2
                        color: "black"
                        radius: 2
                    }
                    ColumnLayout{
                        Layout.preferredHeight:  parent.height
                        Layout.preferredWidth: 0.7*parent.width
                        RowLayout{
                            Layout.preferredHeight:  0.5*parent.height
                            Layout.preferredWidth: parent.width
                            Text { id:extra_text ; text: extra; font.pixelSize: pt2px(11);}
                        }
                        Rectangle{
                            Layout.preferredHeight:  2
                            Layout.preferredWidth:   parent.width-5
                            color: "black"
                            radius: 2
                        }
                        RowLayout{
                            Layout.preferredWidth: parent.width
                            Layout.preferredHeight:  0.5*parent.height
                            Rectangle{
                                Layout.preferredHeight:  parent.height
                                Layout.preferredWidth: 0.3*parent.width
                                color: "transparent";
                                Text { id:fx_text ; text: fx; font.pixelSize: pt2px(11);}
                            }
                            Rectangle{
                                Layout.preferredHeight:  parent.height-5
                                Layout.preferredWidth:   2
                                color: "black"
                                radius: 2
                            }
                            Rectangle{
                                Layout.preferredHeight:  parent.height
                                Layout.preferredWidth: 0.3*parent.width
                                color: "transparent";
                                Text { id:fy_text ; text: fy; font.pixelSize: pt2px(11);}
                            }
                            Rectangle{
                                Layout.preferredHeight:  parent.height-5
                                Layout.preferredWidth:   2
                                color: "black"
                                radius: 2
                            }
                            Rectangle{
                                Layout.preferredHeight:  parent.height
                                Layout.preferredWidth:  0.3*parent.width
                                color: "transparent";
                                Text { id:fz_text ; text: fz; font.pixelSize: pt2px(11);}
                            }
                        }
                    }
                }
            }
        }

        id: listView
        clip:true
        spacing: 2
        anchors.fill: parent
        delegate:delegate
        //highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
        flickableChildren: MouseArea {
            anchors.fill: parent
            onClicked: gridview.currentIndex = -1
        }
        onCurrentIndexChanged: console.log(currentIndex)
    }
}
