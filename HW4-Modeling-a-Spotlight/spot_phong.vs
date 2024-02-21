#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec4 Position;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{


    Normal = normalize( mat3(view*model) * VertexNormal);
    Position = view*model*VertexPosition;

    gl_Position = projection*view*model*VertexPosition;
}
