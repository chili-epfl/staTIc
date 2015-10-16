import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity{
    property real ratioNL:25

    property string type: "External"

    property real myAngle:0;
    property vector3d position: Qt.vector3d(0,0,0)
    property bool visible: true
    property bool isPointingAtPosition:true;
    property real arrowLength:ratioNL;
    property bool highlighted: true;


    /*Internal*/
    property real coneLength: 18
    property bool tooSmall: false
    property real scaleFactor: 1
    property real minThreshold: 0.01

    objectName:"FBD arrow"

    onTypeChanged: {
        if(type=="External"){
            material.ambient= "black"
            material.diffuse= "#b11414"//red
            material.specular= "black"
        }
        if(type=="Support"){
            material.ambient= "black"
            material.diffuse= "#1430b1"//blue
            material.specular= "black"
        }
        if(type=="Internal"){
            material.ambient= "black"
            material.diffuse= "#14b130"//green
            material.specular= "black"
        }
    }



    signal changeIsPointingAtPosition (bool val)
    onChangeIsPointingAtPosition: isPointingAtPosition=val;

    signal changeHighlighted (bool val)
    onChangeHighlighted: highlighted=val

    signal changeVisible (bool val)
    onChangeVisible: visible=val;

    signal changePosition (vector3d pos)
    onChangePosition: position=pos

    signal changeArrowLength (real val)
    onChangeArrowLength: {
        arrowLength=val*ratioNL;

    }

    signal changeMyAngle (real val)
    onChangeMyAngle: {
        myAngle=val;
    }

    onArrowLengthChanged: {
        if(arrowLength > minThreshold)
            tooSmall=false
        else
            tooSmall=true
    }


    PhongMaterial{
        id:material
        ambient:highlighted ? "black" : "#848484"
        diffuse:"#b11414"
        specular:"black"
    }

    Mesh{
        id:tip_mesh
        source:"qrc:/icons/icons/arrow_ble.obj"
        enabled:  visible && !tooSmall;
    }
    Transform{
        id:tip_transform_TailOnPos
        Translate{
              dx:-(arrowLength+coneLength);
        }
        Rotate{
            axis:Qt.vector3d(0,0,1)
            angleRad: 3.14+myAngle
        }
        Scale{
         scale: scaleFactor;
        }
        Translate{
             translation: position
        }

    }

    Transform{
        id:tip_transform_TipOnPos
        Rotate{
            axis:Qt.vector3d(0,0,1)
            angleRad: 3.14+myAngle
        }
        Scale{
         scale: scaleFactor;
        }
        Translate{
            translation: position
        }

    }

    Entity{
        id:tail
        objectName: parent.objectName
        //objectName: "FBD tail"
        CylinderMesh{
            id:tail_mesh
            radius: 4
            length: arrowLength
            enabled:  visible && !tooSmall;
        }

        Transform{
            id:tail_transform
            Rotate{
                axis:Qt.vector3d(0,0,1)
                angle: 90
            }
            Translate{
                dx:coneLength+arrowLength/2
            }
        }

        //components: [tail_mesh,material,tail_transform]

    }


    components: isPointingAtPosition ? [tip_mesh,material,tip_transform_TipOnPos] : [tip_mesh,material,tip_transform_TailOnPos]

}




