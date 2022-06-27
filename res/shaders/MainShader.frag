#version 460
precision mediump float;
out vec4 fragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
  vec4 position;

  vec3 color;
  float intensity;

  float constant;
  float linear;
  float quadratic;
};

layout(std430, binding = 10) buffer lightSSBO
{
    Light lights[];
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};
uniform Material material;

struct Camera {
  vec3 CamPos;
};
uniform Camera camera;

float gamma = 2.2;

vec3 CalcLight(Light light, vec3 normal, vec3 viewDir);

void main()
{
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(camera.CamPos - FragPos);

  vec3 Result = vec3(texture(material.texture_diffuse1, TexCoords)) * 0.3;

  for(int i = 0; i < lights.length(); i++) {
    Result += CalcLight(lights[i], norm, viewDir);
  }

  fragColor = vec4(Result, 1.0);
}

vec3 CalcLight(Light light, vec3 normal, vec3 viewDir) {
  vec3 lightDir;
  float attenuation = 1.0;

  if(light.position.w == 0) {
    // Dir Light
    lightDir = normalize(-light.position.xyz);
  } else {
    // Point Light
    lightDir = normalize(light.position.xyz - FragPos);
    float dist = length(light.position.xyz - FragPos);
    attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
  }

  vec3 halfwayDir = normalize(lightDir + viewDir);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);

  vec3 lightCol = light.color * light.intensity;

  vec3 diffuse  = lightCol * diff * vec3(pow(texture(material.texture_diffuse1, TexCoords).xyz, vec3(gamma)));
  /* diffuse = normal; */
  /* diffuse = FragPos; */
  /* diffuse = vec3(lightDir); */
  vec3 specular = lightCol * spec * vec3(texture(material.texture_specular1, TexCoords).rrr);

  diffuse *= attenuation;
  specular *= attenuation;

  return (diffuse + specular);
}

