#version 460
precision mediump float;
out vec4 fragColor;

in VSOUT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
  mat3 TBN;
} FSIn;

struct Light {
  vec4 Position;

  vec3 Color;
  float Intensity;

  float Constant;
  float Linear;
  float Quadratic;
};

struct Material {
    sampler2D T_Diffuse1;
    sampler2D T_Specular1;
    sampler2D T_Normal1;
};

struct Camera {
  vec3 CamPos;
};

layout(std430, binding = 10) buffer lightSSBO
{
    Light Lights[];
};
uniform Material MeshMat;
uniform Camera MainCam;

float Gamma = 2.2;

vec3 CalcLight(Light Light, vec3 Normal, vec3 ViewDir);

void main()
{
  // Calculate the surface normal using the first normal map and the TBN
  // matrix.
  vec3 Normal = texture(MeshMat.T_Normal1, FSIn.TexCoords).rgb;
  Normal = Normal * 2.0 - 1.0;
  Normal = normalize(FSIn.TBN * Normal);

  // Uncomment to test without normal map.
  /* Normal = normalize(FSIn.Normal); */

  // The direction from the camera to the fragment world-space position.
  vec3 ViewDir = normalize(MainCam.CamPos - FSIn.FragPos);

  // The resulting color.
  // Set to the first diffuse texture with a small coefficient. This simulates
  // ambient light.
  vec3 Result = vec3(texture(MeshMat.T_Diffuse1, FSIn.TexCoords)) * 0.01;

  // Loop through all lights and calculate the lights change to the fragment.
  for(int i = 0; i < Lights.length(); i++) {
    Result += CalcLight(Lights[i], Normal, ViewDir);
  }

  // Set the final color.
  fragColor = vec4(Result, 1.0);
}

vec3 CalcLight(Light Light, vec3 Normal, vec3 ViewDir) {
  vec3 LightDir;
  float Attenuationr = 1.0;

  // Check the w-parameter of the position vector, to determine light type.
  if(Light.Position.w == 0) {
    // Dir Light
    // The position is actually the direction.
    LightDir = normalize(-Light.Position.xyz);
  } else {
    // Point Light
    // The direction is from the position to the fragment position.
    LightDir = normalize(Light.Position.xyz - FSIn.FragPos);
    // Calc distance between position and fragment position. Use for
    // calculating attenuation.
    float Dist = length(Light.Position.xyz - FSIn.FragPos);
    Attenuationr = 1.0 / (Light.Constant + Light.Linear * Dist + Light.Quadratic * (Dist * Dist));
  }

  vec3 HalfwayDir = normalize(LightDir + ViewDir);

  float Diff = max(dot(Normal, LightDir), 0.0);

  vec3 ReflectDir = reflect(-LightDir, Normal);
  float Spec = pow(max(dot(Normal, HalfwayDir), 0.0), 16);

  vec3 LightCol = Light.Color * Light.Intensity;

  vec3 Diffuse  = LightCol * Diff * vec3(pow(texture(MeshMat.T_Diffuse1, FSIn.TexCoords).xyz, vec3(Gamma)));
  vec3 Specular = LightCol * Spec * vec3(texture(MeshMat.T_Specular1, FSIn.TexCoords).rrr);

  Diffuse *= Attenuationr;
  Specular *= Attenuationr;

  return (Diffuse + Specular);
}

