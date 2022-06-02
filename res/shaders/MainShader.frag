#version 460
precision mediump float;
out vec4 fragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main()
{
  vec3 norm = normalize(Normal);
  vec3 lightDir = -normalize(vec3(0.74, 1.346, 1.75));

  float diff = max(dot(norm, -lightDir), 0.0);
  diff = min(max(0.3, diff * 1.5), 1.0);

  vec3 diffuse = texture(material.texture_diffuse1, TexCoords).xyz * diff;

  fragColor = vec4(diffuse, 1.0);
}
