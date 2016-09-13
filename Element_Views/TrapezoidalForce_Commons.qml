import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    property PhongMaterial material_for_selection:
        PhongMaterial {
        ambient:  "yellow"
        diffuse:"grey"
        specular:"black"
        //alpha:0.80
    }

}
