#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 TexCoords;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
    TexCoords = aPos;
    gl_Position = Projection * View * vec4(aPos, 1.0);
}
