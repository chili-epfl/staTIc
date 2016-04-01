import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtPhysics.unofficial 1.0
import QuickTangibleTPZLoad 1.0
import QtQuick 2.3 as QQ2
import ArucoObject 1.0
Entity {
    id:rootEntity
    readonly property string type: "trapezoidalForceTangible"

    property vector3d globalForce: Qt.vector3d(0,-weight*9800,0);
    property vector3d globalPosition:collisionTransform.translation

    property vector3d prev_position:Qt.vector3d(0,0,0)
    property size tangibleSection: "0x0"
    signal positionChanged();

    onGlobalPositionChanged: {
        if(prev_position.minus(globalPosition).length()>10){
            prev_position=globalPosition
            positionChanged();
        }
    }

    property url asset3DMeshURL:"qrc:/tools/Tools/TangibleTrapzLoad/tonweight.obj";
    property url asset3DTextureURL:"qrc:/tools/Tools/TangibleTrapzLoad/texture.png";
    property real weight:10;
    property vector2d extent: Qt.vector2d(-10,10)

    property bool selected: infobox.current_item==rootEntity? true: false

    ArucoObject{
       id:tag
       objectId: "200"
       QQ2.Component.onCompleted: aruco.registerObserver(this)
    }

    /*Graphical rapresentation*/
    SphereMesh{
        id:collisionSphere
        enabled: true
        radius:15
    }
    Transform{
        id:collisionTransform
        matrix: {
            var m = Qt.matrix4x4();
            //m.rotate(-180, Qt.vector3d(1, 0, 0))
            m=m.times(structureLoaderTransform.matrix.inverted().times(tag.poseMatrix))
            //m.rotate(180, Qt.vector3d(1, 0, 0))
            return m;
        }

    }
    PhongAlphaMaterial{
        id:transparentMaterial
        alpha:0.5
    }
    PhysicsBodyInfo{
        id:sphereBody
        kinematic:true
        inputTransform: collisionTransform;
    }

    QuickTangibleTPZLoad{
        id: quickTPZ
        vmManager:vmFrame3DDManager
        emittingBodyInfo:sphereBody
        parentEntity:rootEntity

    }
    ObjectPicker {
        id:objectPicker
        hoverEnabled: false
        onClicked: {
            if(applicationRoot.currentViewFilter=='DESIGNER')
                infobox.current_item=rootEntity
        }
    }

    EntityLoader{
        id:entity_loader
        source:"qrc:/tools/Tools/TangibleTrapzLoad/TangibleTrapzLoad_InjectedEntity.qml"
        QQ2.Binding{
            target: entity_loader.entity
            when: entity_loader.entity
            property:"offsetAugmentation"
            value: Qt.vector3d(tangibleSection.height/2,0,-tangibleSection.width/2)
        }
        QQ2.Binding{
            target: entity_loader.entity
            when: entity_loader.entity
            property:"selected"
            value: selected
        }
        QQ2.Binding{
            target: entity_loader.entity
            when: entity_loader.entity
            property:"asset3DTextureURL"
            value: asset3DTextureURL
        }
        QQ2.Binding{
            target: entity_loader.entity
            when: entity_loader.entity
            property:"asset3DMeshURL"
            value: asset3DMeshURL
        }

    }

    components:[objectPicker,collisionSphere,transparentMaterial,sphereBody,collisionTransform]

}
