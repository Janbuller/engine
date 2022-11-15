#version 460 core
layout (location = 0) out vec3 GPos;
layout (location = 1) out vec3 GNorm;
layout (location = 2) out vec4 GCol;

in vec3 TexCoords;

uniform samplerCube Skybox;

void main()
{    
    GCol = texture(Skybox, TexCoords);
    GPos = vec3(1.0/0.0);
}
