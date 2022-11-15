#version 460
layout (location = 0) out vec3 GPos;
layout (location = 1) out vec3 GNorm;
layout (location = 2) out vec4 GCol;

in VSOUT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
  mat3 TBN;
} FSIn;

struct Material {
  sampler2D T_Diffuse1;
  sampler2D T_Specular1;
  sampler2D T_Normal1;
};

uniform Material MeshMat;

float Gamma = 2.2;

void main()
{    
    GPos = FSIn.FragPos;

    vec3 Normal = texture(MeshMat.T_Normal1, FSIn.TexCoords).rgb;
    Normal = Normal * 2.0 - 1.0;
    Normal = normalize(FSIn.TBN * Normal);

    GNorm = Normal;

    GCol.rgb = vec3(pow(texture(MeshMat.T_Diffuse1, FSIn.TexCoords).xyz, vec3(Gamma)));
    GCol.a   = texture(MeshMat.T_Specular1, FSIn.TexCoords).r;
    return;
}  
