
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: alphaMaterial

    effect:  Effect {

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
//                        renderStates: [
//                            CullFace { mode : CullFace.Back },
//                            DepthTest { func: DepthTest.Less },
//                            DepthMask { mask: false },
//                            BlendState {
//                                srcRGB: BlendState.SrcAlpha
//                                dstRGB: BlendState.OneMinusSrcAlpha
//                            }
//                            ,BlendEquation {mode: BlendEquation.FuncAdd}
//                        ]
                        shaderProgram: alphaPhong
                    }
                ]

           },
            Technique
            {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGLES
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
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
