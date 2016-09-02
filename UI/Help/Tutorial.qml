import QtQuick 2.0
import QtQuick.Controls 1.4
Item {
    signal exitTutorial();
    anchors.fill: parent

    MouseArea{
        anchors.fill: parent
    }

    property var pages: [
        {
            "description":"StaTIc is an Augmented Reality app for exploring the behaviour of structures"
            ,"imageUrl":"qrc:/ui/UI/Help/page_1.jpg"
        }
        ,{
            "description":"Point the camera at the structure to see the information.<br> The quality of the augmentation depends
                            on the number of visible markers.<br>
                            If the image is over/under exposed use the slider on the bottom left to adjust the detection."
            ,"imageUrl":"qrc:/ui/UI/Help/page_2.gif"
        }
        ,{
            "description":"In the main view, axial loads are displayed on the structure elements by springs and arrows.<br>
                           Notice that the arrows <b>point toward</b> the joint when the beam is under <b>compression</b>, whereas they <b>point outwards</b> when the beam is in <b>tension</b>."
            ,"imageUrl":"qrc:/ui/UI/Help/page_3.gif"
        }
        ,{
            "description":"Remeber that a structure is in equilibrium when the sum of the forces and the total momentum at the joints is zero.<br>
                            The arrows represents the contribution of the beam to the equilibrium of the joint."
            ,"imageUrl":"qrc:/ui/UI/Help/page_4.gif"
        }
        ,{
            "description":"Some joints must be constrainted in order to have a stable structure.<br> The supports can be fixed, pinned or rolling.<br>
                The reaction from the supports are displayed by green arrows."
            ,"imageUrl":"qrc:/ui/UI/Help/page_5.png"
        }
        ,{
            "description":"In the <b>design mode</b> you can add load on the beams"
            ,"imageUrl":"qrc:/ui/UI/Help/page_6.gif"
        }
        ,{
            "description":"you can change the material and the section of the beams"
            ,"imageUrl":"qrc:/ui/UI/Help/page_7.gif"
        }
        ,{
            "description":"you can change the support at the joints"
            ,"imageUrl":"qrc:/ui/UI/Help/page_8.gif"
        }
        ,{
            "description":"In the <b>analysis mode</b> click on a beam to see the behavoir (bending, displacement, etc.) and exagerate it."
            ,"imageUrl":"qrc:/ui/UI/Help/page_9.gif"
        }
        ,{
            "description":"or click on a joint to explore the composition of the forces from the connected elements by adding or removing the single contributions."
            ,"imageUrl":"qrc:/ui/UI/Help/page_10.gif"
        }
        ,{
            "description":"Check the stress percentage in the structure.<br>
                            Stess percentages are computed based on the limits due to the material and the section of structural elements."
            ,"imageUrl":"qrc:/ui/UI/Help/page_11.gif"
        }
        ,{
            "description":"Check the displacement of the nodes and exagerate it."
            ,"imageUrl":"qrc:/ui/UI/Help/page_12.gif"
        }
    ]


    Rectangle{
        anchors.centerIn: parent
        width: parent.width*0.8
        height: parent.height*0.8
        color:"#2f3439"
        border.width: 5
        border.color: "#F0F0F0"

        StackView{
            width: parent.width-20
            height: parent.height*0.75 -20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: navigation_bar.top
            id:stack
            initialItem: tutorial_window
        }

        Component{
            id:tutorial_window
            Column{
                spacing: 20
                Text {
                    text: pages[stack.depth-1].description
                    width: parent.width*0.8
                    height: parent.height*0.3
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.WordWrap
                    fontSizeMode: Text.Fit
                    font.pointSize: 20
                    color: "white"
                    verticalAlignment: Image.AlignVCenter
                }
                AnimatedImage {
                    id:animated_image
                    source: pages[stack.depth-1].imageUrl
                    width: parent.width*0.7
                    height: parent.height*0.7
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter: parent.horizontalCenter
                    playing: stack.busy==false
                    onStatusChanged:
                        if(status == AnimatedImage.Ready)
                            repeter_gif.model=frameCount
                    Row{
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        Repeater{
                            id:repeter_gif
                            anchors.centerIn: parent
                            anchors.fill: parent
                            delegate: Item{
                                width: 15
                                height: 15
                                Rectangle{
                                    anchors.centerIn: parent
                                    width: parent.width-5
                                    height: width
                                    radius: width/2
                                    color: index!=animated_image.currentFrame ? "#0a242d":"#33b5e5"
                                }
                            }
                        }

                    }
                }

            }
        }

        Item{
            id:navigation_bar
            width: parent.width
            height: parent.height*0.2
            anchors.bottom: parent.bottom
            Button{
                id: skip_button
                text:"Skip"
                onClicked: exitTutorial()
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 10
                anchors.left: parent.left
            }
            Row{
                anchors.centerIn: parent
                Repeater{
                    anchors.centerIn: parent
                    anchors.fill: parent
                    model:pages.length
                    delegate: Item{
                        width: 25
                        height: 25
                        Rectangle{
                            anchors.centerIn: parent
                            width: parent.width-5
                            height: width
                            radius: width/2
                            color: index!=stack.depth-1 ? "#0a242d":"#33b5e5"
                        }
                    }
                }

            }

            Button{
                id:next_button
                text:"Next"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 10
                onClicked: if(stack.depth<pages.length)
                               stack.push({item:tutorial_window,immediate:true})
                           else exitTutorial();
            }
            Button{
                id:back_button
                text:"Back"
                anchors.right: next_button.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 10
                onClicked: if(stack.depth>1) stack.pop({immediate: true})
            }
        }




    }







}
