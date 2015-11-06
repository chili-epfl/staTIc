import Qt3D 2.0
import Qt3D.Renderer 2.0
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
            name: "tag_82"
        }
        SphereMesh{
            id:extreme1Mesh
            radius: 10
        }
        Transform{
          id:extreme1Transform
          Rotate{

              axis: Qt.vector3d(1, 0, 0)
              angle: -180
          }
          MatrixTransform{
             matrix: sceneloadermatrix.matrix.inverted().times(extreme1Tag.transform)
          }
          Rotate{
              axis: Qt.vector3d(1, 0, 0)
              angle: 180
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
            name: "tag_87"
        }
        SphereMesh{
            id:extreme2Mesh
            radius: 10
        }
        Transform{
          id:extreme2Transform
          Rotate{

              axis: Qt.vector3d(1, 0, 0)
              angle: -180
          }
          MatrixTransform{
             matrix: sceneloadermatrix.matrix.inverted().times(extreme2Tag.transform)
          }
          Rotate{
              axis: Qt.vector3d(1, 0, 0)
              angle: 180
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

