import Qt3D.Core 2.0
import Qt3D.Render 2.0
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
        name: "tag_7"
    }
    /*Graphical rapresentation*/
    SphereMesh{
        id:collisionSphere
        enabled: false
        radius:15
    }
    Transform{
        id:collisionTransform
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(-180, Qt.vector3d(1, 0, 0))
            m*=structure_tag.transform.inverted().times(forceTag.transform)
            m.rotate(180, Qt.vector3d(1, 0, 0))
            return m;
        }
    }

    PhysicsBodyInfo{
        id:sphereBody
        kinematic:true
        inputTransform: collisionTransform;
    }

    QuickConcentratedForce{
        id: quickCF
        vmManager:vmFrame3DDManager
        emittingBodyInfo:sphereBody
    }

    components:[collisionSphere,sphereBody,collisionTransform]

}





