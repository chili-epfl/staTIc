
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Material {
    id: alphaMaterial
    property int stencilValue:0
    //onStencilValueChanged: console.log(stencilValue)
    effect:  Effect {

        ShaderProgram {
            id: trivialGL3
            vertexShaderCode: loadSource("qrc:/opengl/Opengl/TrivialGL3.vert")
            fragmentShaderCode: loadSource("qrc:/opengl/Opengl/TrivialGL3.frag")
        }
        ShaderProgram {
            id: trivialES2
            vertexShaderCode: loadSource("qrc:/opengl/Opengl/TrivialES2.vert")
            fragmentShaderCode: loadSource("qrc:/opengl/Opengl/TrivialES2.frag")
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
                filterKeys: [FilterKey { name: "renderingStyle"; value: "ar" } ]
                renderPasses: [
                    RenderPass {
                        renderStates: [
                            CullFace { mode : CullFace.Front },
                            DepthTest{ depthFunction: DepthTest.Less }
//                            ,StencilMask{
//                                backOutputMask: ~0
//                                frontOutputMask: ~0
//                            },
//                            StencilTest{
//                                front.stencilFunction: StencilTestArguments.Always
//                                front.referenceValue:stencilValue
//                                front.comparisonMask: ~0
//                                back.stencilFunction: StencilTestArguments.Always
//                                back.referenceValue:stencilValue
//                                back.comparisonMask: ~0
//                            },
//                            StencilOperation{
//                                front.allTestsPassOperation: StencilOperationArguments.Replace
//                                back.allTestsPassOperation: StencilOperationArguments.Replace
//                            }
                        ]
                        shaderProgram: trivialGL3
                    }
                ]

           },
            Technique
            {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
                }

                filterKeys: [FilterKey { name: "renderingStyle"; value: "ar" } ]
                renderPasses: [
                    RenderPass {
                        renderStates: [
                            CullFace { mode : CullFace.Front },
                            DepthTest{ depthFunction: DepthTest.Less }
//                            ,StencilMask{
//                                backOutputMask: ~0
//                                frontOutputMask: ~0
//                            },
//                            StencilTest{
//                                front.stencilFunction: StencilTestArguments.Always
//                                front.referenceValue:stencilValue
//                                front.comparisonMask: ~0
//                                back.stencilFunction: StencilTestArguments.Always
//                                back.referenceValue:stencilValue
//                                back.comparisonMask: ~0
//                            },
//                            StencilOperation{
//                                front.allTestsPassOperation: StencilOperationArguments.Replace
//                                back.allTestsPassOperation: StencilOperationArguments.Replace
//                            }
                        ]
                        shaderProgram: trivialES2
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

                filterKeys: [FilterKey { name: "renderingStyle"; value: "ar" } ]
                renderPasses: [
                    RenderPass {
                        renderStates: [
                            CullFace { mode : CullFace.Front },
                            DepthTest{ depthFunction: DepthTest.Less }
//                            ,StencilMask{
//                                backOutputMask: ~0
//                                frontOutputMask: ~0
//                            },
//                            StencilTest{
//                                front.stencilFunction: StencilTestArguments.Always
//                                front.referenceValue:stencilValue
//                                front.comparisonMask: ~0
//                                back.stencilFunction: StencilTestArguments.Always
//                                back.referenceValue:stencilValue
//                                back.comparisonMask: ~0
//                            },
//                            StencilOperation{
//                                front.allTestsPassOperation: StencilOperationArguments.Replace
//                                back.allTestsPassOperation: StencilOperationArguments.Replace
//                            }
                        ]
                        shaderProgram: trivialES2
                    }
                ]
           }
        ]
    }
}
