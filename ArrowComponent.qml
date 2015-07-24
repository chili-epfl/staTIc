import Qt3D 2.0
import Qt3D.Renderer 2.0


Entity{
    property real myAngle:0;
    property real arrowLength:10;
    property real positionX: 0;
    property real positionY: 0;
    property real positionZ: 0;
    property real coneLength: 18
    Mesh{
        id:tip_mesh
        source:"qrc:/icons/icons/arrow_ble.obj"
    }
    Transform{
        id:tip_transform
        Rotate{
            axis:Qt.vector3d(0,0,1)
            angleRad: myAngle
        }
        Translate{
              dx:positionX;
              dy:positionY;
              dz:positionZ;
        }

    }
    Entity{
        objectName: parent.objectName
        CylinderMesh{
            id:tail_mesh

            radius: 10
            length: arrowLength
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
    components: [tip_mesh,floorMaterial,tip_transform]
}

