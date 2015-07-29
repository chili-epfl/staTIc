import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity {

    property vector3d position;

    components: [
       Transform{
         Translate{
            translation: position
         }
        },
        SphereMesh{
         radius: 3
        }
        ]

}

