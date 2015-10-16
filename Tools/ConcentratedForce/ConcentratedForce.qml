import Qt3D 2.0
import Qt3D.Renderer 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0
import QuickConcentratedForce 1.0
import QtQuick 2.3 as QQ2

Entity {
    objectName: "Force"

    //property matrix4x4 pose:forceTag.matrix
//    property alias chilitag: forceTag

     /*Graphical rapresentation*/
       SphereMesh{
          id:collitionSphere
          radius:20
       }
       Transform{
         id:collitionTransform
         Rotate{
             axis: Qt.vector3d(1, 0, 0)
             angle: -180
         }
         MatrixTransform{
            matrix: sceneloadermatrix.matrix.inverted().times(forceTag.transform)
         }
         Rotate{
             axis: Qt.vector3d(1, 0, 0)
             angle: 180
         }
       }

       PhysicsBodyInfo{
           id:sphereBody
           kinematic:true
           inputTransform: collitionTransform;
       }

       QuickConcentratedForce{
            id: quickCF
            uiManager:eventhandler
            emittingBodyInfo:sphereBody
       }

       components:[collitionSphere,sphereBody,collitionTransform]

}





