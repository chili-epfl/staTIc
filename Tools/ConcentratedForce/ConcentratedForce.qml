import Qt3D 2.0
import Qt3D.Renderer 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0
import QuickConcentratedForce 1.0

Entity {

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
            matrix: forceTag.transform
          }
         Rotate{
             axis: Qt.vector3d(1, 0, 0)
             angle: 180
         }
       }

       PhysicsBodyInfo{
           id:sphereBody
           kinematic:true
           inputTransform: collitionTransform
           onCollided: {quickCF.onCollition(event)}

       }

       QuickConcentratedForce{
            id: quickCF
            uiManager:eventhandler
       }

       components:[collitionSphere,sphereBody,collitionTransform]

}





