
import Qt3D 2.0
import Qt3D.Renderer 2.0

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
            vertexShaderCode: loadSource("qrc:/opengl/opengl/phongalpha.vert")
            fragmentShaderCode: loadSource("qrc:/opengl/opengl/phongalpha.frag")
        }

        techniques: [
            Technique
            {
                openGLFilter {
                    api: OpenGLFilter.Desktop
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }

                renderPasses: RenderPass {
                    renderStates: [
                        CullFace { mode : CullFace.Back },
                        DepthTest { func: DepthTest.Less },
                        DepthMask { mask: false },
                        BlendState {
                            srcRGB: BlendState.SrcAlpha
                            dstRGB: BlendState.OneMinusSrcAlpha
                        }
                        ,BlendEquation {mode: BlendEquation.FuncAdd}
                    ]

                    shaderProgram: alphaPhong
                }
            },
            Technique
            {
                openGLFilter {
                    api: OpenGLFilter.ES
                    profile: OpenGLFilter.None
                    majorVersion: 2
                    minorVersion: 0
                }

                renderPasses: RenderPass {
                    renderStates: [
                        CullFace { mode : CullFace.Back },
                        DepthTest { func: DepthTest.Less },
                        DepthMask { mask: false },
                        BlendState {
                            srcRGB: BlendState.SrcAlpha
                            dstRGB: BlendState.OneMinusSrcAlpha
                        }
                        ,BlendEquation {mode: BlendEquation.FuncAdd}
                    ]

                    shaderProgram: alphaPhong
                }
            }
        ]
    }
}
