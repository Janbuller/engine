#version 460
precision mediump float;
out vec4 fragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

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
    sampler2D texture_normal1;
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
  vec3 Norm = texture(material.texture_normal1, TexCoords).rgb;
  Norm = Norm * 2.0 - 1.0;
  Norm = normalize(TBN * Norm);
  /* Norm = normalize(Normal); */

  vec3 ViewDir = normalize(camera.CamPos - FragPos);

  vec3 Result = vec3(texture(material.texture_diffuse1, TexCoords)) * 0.01;

  for(int i = 0; i < lights.length(); i++) {
    Result += CalcLight(lights[i], Norm, ViewDir);
  }

  fragColor = vec4(Result, 1.0);
}

vec3 CalcLight(Light light, vec3 normal, vec3 viewDir) {
  vec3 LightDir;
  float Attenuationr = 1.0;

  // Check the w-parameter of the position vector, to determine light type.
  if(light.position.w == 0) {
    // Dir Light
    // The position is actually the direction.
    LightDir = normalize(-light.position.xyz);
  } else {
    // Point Light
    // The direction is from the position to the fragment position.
    LightDir = normalize(light.position.xyz - FragPos);
    // Calc distance between position and fragment position. Use for
    // calculating attenuation.
    float Dist = length(light.position.xyz - FragPos);
    Attenuationr = 1.0 / (light.constant + light.linear * Dist + light.quadratic * (Dist * Dist));
  }

  vec3 HalfwayDir = normalize(LightDir + viewDir);

  float Diff = max(dot(normal, LightDir), 0.0);

  vec3 ReflectDir = reflect(-LightDir, normal);
  float Spec = pow(max(dot(normal, HalfwayDir), 0.0), 16);

  vec3 LightCol = light.color * light.intensity;

  vec3 Diffuse  = LightCol * Diff * vec3(pow(texture(material.texture_diffuse1, TexCoords).xyz, vec3(gamma)));
  vec3 Specular = LightCol * Spec * vec3(texture(material.texture_specular1, TexCoords).rrr);

  Diffuse *= Attenuationr;
  Specular *= Attenuationr;

  return (Diffuse + Specular);
}

