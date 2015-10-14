import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity {

    property vector3d position;
    property bool visible: false

    signal changeVisible (bool val)
    onChangeVisible: {
        visible=val
        if(visible){
            components=[transform,mesh]
        }
        else{
            components=[]
        }
    }

    objectName: "FBD center"
    Transform{
        id:transform
        Translate{
            translation: position
        }
    }
    SphereMesh{
        id:mesh
        radius: 3
        enabled: visible
    }


}

