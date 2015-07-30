import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity{
    property real ratioNL:50
    property real scaleFactor: 0.5
    property real minThreshold: 0.01
    property bool tooSmall: false

    property real myAngle:0;
    property real arrowLength:ratioNL;
    property vector3d position: Qt.vector3d(0,0,0)
    property bool visible: true
    property bool isPointingAtPosition:true;

    signal changeIsPointingAtPosition (bool val)
    onChangeIsPointingAtPosition: isPointingAtPosition=val;

    signal changeVisible (bool val)
    onChangeVisible: visible=val

    signal changePosition (vector3d pos)
    onChangePosition: position=pos

    signal changeArrowLength (real val)
    onChangeArrowLength: { arrowLength=val*ratioNL; }

    signal changeMyAngle (real val)
    onChangeMyAngle: myAngle=val

    onArrowLengthChanged: {arrowLength > minThreshold ? tooSmall=false: tooSmall=true}

    property real coneLength: 18

    Mesh{
        id:tip_mesh
        source:"qrc:/icons/icons/arrow_ble.obj"
        enabled: visible && !tooSmall;
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
        objectName: parent.objectName
        CylinderMesh{
            id:tail_mesh
            radius: 10
            length: arrowLength
            enabled: visible && !tooSmall;
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

        components: [tail_mesh,tail_transform]

    }

    PhongMaterial {
            id: floorMaterial
             ambient: "red"
             diffuse: "red"
             specular: "red"
             shininess: 0.5
    }
    components: isPointingAtPosition ? [tip_mesh,floorMaterial,tip_transform_TipOnPos] : [tip_mesh,floorMaterial,tip_transform_TailOnPos]

}




