#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VSOUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} VSOut;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    VSOut.FragPos = vec3(Model * vec4(aPos, 1.0));

    mat4 PVM = Projection * View * Model;

    VSOut.Normal = mat3(transpose(inverse(Model))) * aNormal;  

    VSOut.TexCoords = aTexCoords;

    // Calculate the T and N vectors based on the vertex inputs and the model
    // matrix.
    vec3 T = normalize(vec3(Model * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(Model * vec4(aNormal,    0.0)));
    // Reorthogonalize T vector based on the N vector.
    T = normalize(T - dot(T, N) * N);
    // Calculate the B Vector.
    vec3 B = cross(N, T);
    VSOut.TBN = mat3(T, B, N);

    gl_Position = PVM * vec4(aPos, 1.0);
}
