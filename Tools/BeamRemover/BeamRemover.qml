import Qt3D 2.0
import Qt3D.Renderer 2.0
import Chilitags 1.0
import QtPhysics.unofficial 1.0
import QuickBeamRemover 1.0
import QtQuick 2.3 as QQ2

Entity {

    QQ2.Component.onCompleted: {
        var array=chilitags.chiliobjects
        array.push(forceTag)
        chilitags.chiliobjects=array;
    }
    ChilitagsObject{
        id:beamRemoverTag
        name: "tag_1022"
    }
    /*Graphical rapresentation*/
    SphereMesh{
        id:collitionSphere
        radius:10
    }
    Transform{
        id:collitionTransform
        Rotate{
            axis: Qt.vector3d(1, 0, 0)
            angle: -180
        }
        MatrixTransform{
            matrix: structure_tag.transform.inverted().times(beamRemoverTag.transform)
        }
        Rotate{
            axis: Qt.vector3d(1, 0, 0)
            angle: 180
        }
    }

    PhysicsBodyInfo{
        id:collitionBody
        kinematic:true
        inputTransform: collitionTransform;
    }

    QuickBeamRemover{
        id: quickBR
        vmManager:vmFrame3DDManager
        emittingBodyInfo:collitionBody
    }

    components:[collitionSphere,collitionBody,collitionTransform]

}
