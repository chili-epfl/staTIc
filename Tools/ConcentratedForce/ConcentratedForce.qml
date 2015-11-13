import Qt3D 2.0
import Qt3D.Renderer 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0
import QuickConcentratedForce 1.0
import QtQuick 2.3 as QQ2

Entity {
    QQ2.Component.onCompleted: {
        var array=chilitags.chiliobjects
        array.push(forceTag)
        chilitags.chiliobjects=array;
    }
    ChilitagsObject{
        id:forceTag
        name: "tag_1022"
    }
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
            matrix: structure_tag.transform.inverted().times(forceTag.transform)
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
        vmManager:vmFrame3DDManager
        emittingBodyInfo:sphereBody
    }

    components:[collitionSphere,sphereBody,collitionTransform]

}





