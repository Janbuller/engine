#version 460
precision mediump float;
out vec4 fragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
};

uniform Material material;

void main()
{
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(vec3(1.75, 1.346, 0.74));

  float diff = max(dot(norm, lightDir), 0.0);
  diff = min(max(0.3, diff * 1.5), 1.0);

  fragColor = vec4(texture(material.texture_diffuse1, TexCoords).xyz * diff, 1.0);
}
