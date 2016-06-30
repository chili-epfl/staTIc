import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtPhysics.unofficial 1.0
import QuickSnow 1.0
import QtQuick 2.5 as QQ2

Entity{

    PhysicsBodyInfo{
        id:sphereBody
        inputTransform: initialTransform;
        mass: 1
        friction: 5
        restitution: 0
        onCollided: console.log("hit")
    }

    CuboidMesh{
        id:sphereMesh
        xExtent: 50
        yExtent: 50
        zExtent: 50
    }

    Transform{
        id:initialTransform
        translation:Qt.vector3d(-50,500,0)
    }

    QuickSnow{
        id:quick_snow
        vmManager: vmFrame3DDManager

    }
    QQ2.Timer{
        interval: 10000
        onTriggered: quick_snow.findExposedBeams()
        running: true
    }

    components: [sphereMesh,sphereBody,sphereBody.outputTransform]

}
