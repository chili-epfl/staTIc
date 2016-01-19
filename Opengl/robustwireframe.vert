#version 330 core

in vec4 vertexPosition;
in vec3 vertexNormal;

out EyeSpaceVertex {
    vec3 position;
    vec3 normal;
    int batchId;
} vs_out;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
        vs_out.normal = normalize( modelViewNormal * vertexNormal );
        vs_out.position = vec3( modelView * vec4( vec3(vertexPosition),1) );
        vs_out.batchId = int(vertexPosition.w);
        gl_Position = mvp *  vec4( vec3(vertexPosition),1);
}
