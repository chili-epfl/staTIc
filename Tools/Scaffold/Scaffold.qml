import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtPhysics.unofficial 1.0
import QuickScaffold 1.0
import QtQuick 2.3 as QQ2
import ArucoObject 1.0

Entity {
    property ArucoObject extreme1_tag;
    property ArucoObject extreme2_tag;

    components: [
        Transform{
            id:inv_rotation
            rotation:quaternion_helper.invert(structureLoaderTransform.rotation)
        }
    ]
    Entity{
        components: [
            Transform{
                id:inv_transform
                translation:structureLoaderTransform.translation.times(-1)
            }
        ]


        PhongAlphaMaterial{
            id:material
            alpha:1
        }
        Entity{
            SphereMesh{
                enabled: true
                id:extreme1Mesh
                radius: 10
            }
            Transform{
                id:extreme1Transform
                translation:extreme1_tag.translation
            }

            PhysicsBodyInfo{
                id:extreme1Body
                kinematic:true
                inputTransform: extreme1Transform;
            }
            components:[extreme1Body,material,extreme1Mesh,extreme1Transform]
        }
        Entity{

            SphereMesh{
                enabled: true
                id:extreme2Mesh
                radius: 10
            }
            Transform{
                id:extreme2Transform
                translation:extreme2_tag.translation
            }

            PhysicsBodyInfo{
                id:extreme2Body
                kinematic:true
                inputTransform: extreme2Transform;
            }
            components:[extreme2Body,material,extreme2Mesh,extreme2Transform]
        }

        QuickScaffold{
            vmManager:vmFrame3DDManager
            extreme1: extreme1Body
            extreme2: extreme2Body
            localExtreme1Pos:quaternion_helper.rotateVector(inv_rotation.rotation,extreme1_tag.translation.plus(inv_transform.translation))
            localExtreme2Pos:quaternion_helper.rotateVector(inv_rotation.rotation,extreme2_tag.translation.plus(inv_transform.translation))
        }
    }

}

