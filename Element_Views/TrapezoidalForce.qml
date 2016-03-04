import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.3 as QQ2
Entity{
    id:rootEntity
    readonly property string type: "trapezoidalForce"
    property vector3d force: Qt.vector3d(0,-1,0);
    property vector3d begin: Qt.vector3d(0,0,0);
    property vector3d end: Qt.vector3d(0.5,0,0);

    signal relativePositionChanged

    onBeginChanged:  relativePositionChanged;
    onEndChanged: relativePositionChanged()

    property bool dragging: false;
    /*Visual aspect*/
    CuboidMesh{
        id:mesh
        xExtent: 10
        yExtent: 10
        zExtent: 10
    }

    property ObjectPicker objectPicker: ObjectPicker {
        hoverEnabled: false
        onClicked: {
                infobox.current_item=rootEntity
                rootEntity.parent.drag_anchor_enabled=false;
        }
        onPressed: {
            infobox.current_item=rootEntity
            rootEntity.parent.drag_anchor_enabled=true;
            rootEntity.parent.current_anchor_position=transform.translation
            dragging=true;
        }
        onReleased: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
    }
    Transform{
        id:transform
        scale3D:Qt.vector3d(2,2,2)
        QQ2.Binding on translation{
            when: dragging && rootEntity.parent!=null
            value:rootEntity.parent.current_anchor_position
        }
        onTranslationChanged:{
                    resetTimer.restart()
        }
    }

    components: [mesh,objectPicker,transform]
    QQ2.Timer{
        id:resetTimer
        interval: 3000
        onTriggered: {
            rootEntity.parent.drag_anchor_enabled=false;
            dragging=false;
        }
        running: false;
    }
}
