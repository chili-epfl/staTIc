import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity {

    property vector3d position;
    property bool visible: true

    signal changeVisible (bool val)
    onChangeVisible: visible=val

    components: [
       Transform{
         Translate{
            translation: position
         }
        },
        SphereMesh{
         radius: 3
         enabled: visible
        }
        ]

}

