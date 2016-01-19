import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0
import QuickScaffold 1.0
import QtQuick 2.3 as QQ2

Entity {
    QQ2.Component.onCompleted: {
        var array=chilitags.chiliobjects
        array.push(extreme1Tag)
        array.push(extreme2Tag)
        chilitags.chiliobjects=array;
    }

    Entity{
        ChilitagsObject{
            id:extreme1Tag
            name: "tag_5"
        }
        SphereMesh{
            enabled: false
            id:extreme1Mesh
            radius: 20
        }
        Transform{
          id:extreme1Transform
          matrix: {
              var m = Qt.matrix4x4();
              m.rotate(-180, Qt.vector3d(1, 0, 0))
              m*=structure_tag.transform.inverted().times(extreme1Tag.transform)
              m.rotate(180, Qt.vector3d(1, 0, 0))
              return m;
          }

        }
        PhysicsBodyInfo{
            id:extreme1Body
            kinematic:true
            inputTransform: extreme1Transform;
        }
        components:[extreme1Body,extreme1Mesh,extreme1Transform]
    }

    Entity{
        ChilitagsObject{
            id:extreme2Tag
            name: "tag_6"
        }
        SphereMesh{
            enabled: false
            id:extreme2Mesh
            radius: 20
        }
        Transform{
          id:extreme2Transform
          matrix: {
              var m = Qt.matrix4x4();
              m.rotate(-180, Qt.vector3d(1, 0, 0))
              m*=structure_tag.transform.inverted().times(extreme2Tag.transform)
              m.rotate(180, Qt.vector3d(1, 0, 0))
              return m;
          }

        }
        PhysicsBodyInfo{
            id:extreme2Body
            kinematic:true
            inputTransform: extreme2Transform;
        }
        components:[extreme2Body,extreme2Mesh,extreme2Transform]
    }

    QuickScaffold{
        vmManager:vmFrame3DDManager
        extreme1: extreme1Body
        extreme2: extreme2Body
    }


}

