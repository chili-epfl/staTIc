
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: alphaMaterial

    property real alpha: 0.5
    property color ka: "black"
    property color kd : "blue"
    property color ks: "blue"
    property real shininess: 75

    effect:  Effect {
        parameters: [
            Parameter { name: "alpha";  value: alphaMaterial.alpha },
            Parameter { name: "ka";   value: alphaMaterial.ka },
            Parameter { name: "kd";   value: alphaMaterial.kd },
            Parameter { name: "ks";  value: alphaMaterial.ks },
            Parameter { name: "shininess"; value: alphaMaterial.shininess },
            Parameter { name: "lightPosition"; value: Qt.vector4d(0.0,0.0, 1.0, 1.0) },
            Parameter { name: "lightIntensity"; value: Qt.vector3d(1.0, 1.0, 1.0) }
        ]

        ShaderProgram {
            id: alphaPhong
            vertexShaderCode: loadSource("qrc:/opengl/Opengl/phongalpha.vert")
            fragmentShaderCode: loadSource("qrc:/opengl/Opengl/phongalpha.frag")
        }

        techniques: [
            Technique
            {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 1
                }

                annotations: [ Annotation { name: "renderingStyle"; value: "forward" } ]
                renderPasses: [
                    RenderPass {


                        shaderProgram: alphaPhong
                    }
                ]

           }
        ]
    }
}
