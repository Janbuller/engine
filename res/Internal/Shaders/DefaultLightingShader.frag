#version 460
precision mediump float;
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D GPos;
uniform sampler2D GNorm;
uniform sampler2D GCol;

struct Light {
    vec4 Position;

    vec3 Color;
    float Intensity;

    float Constant;
    float Linear;
    float Quadratic;
};

struct Camera {
    vec3 CamPos;
};
uniform Camera MainCam;

layout(std430, binding = 10) buffer lightSSBO
{
    Light Lights[];
};

vec3 CalcLight(Light Light, vec3 FragPos, vec3 Normal, vec3 ViewDir);

void main()
{             
    vec3 FragPos = texture(GPos, TexCoords).xyz;

    // TODO: Rewrite skybox code.
    // Really hacky way of detecting skybox.
    if(FragPos == vec3(1.0/0.0)) {
        FragColor = texture(GCol, TexCoords);
        return;
    }

    vec3 Normal = texture(GNorm, TexCoords).xyz;

    vec3 ViewDir = normalize(MainCam.CamPos - FragPos);

    vec3 Result = vec3(texture(GCol, TexCoords)) * 0.02;

    for(int i = 0; i < Lights.length(); i++) {
        Result += CalcLight(Lights[i], FragPos, Normal, ViewDir);
    }

    FragColor = vec4(Result, 1.0);
}

vec3 CalcLight(Light Light, vec3 FragPos, vec3 Normal, vec3 ViewDir) {
    vec3 LightDir;
    float Attenuation = 1.0;

    // Check the w-parameter of the position vector, to determine light type.
    if(Light.Position.w == 0) {
        // Dir Light
        // The position is actually the direction.
        LightDir = normalize(-Light.Position.xyz);
    } else {
        // Point Light
        // The direction is from the position to the fragment position.
        LightDir = normalize(Light.Position.xyz - FragPos);
        // Calc distance between position and fragment position. Use for
        // calculating attenuation.
        float Dist = length(Light.Position.xyz - FragPos);
        Attenuation = 1.0 / (Light.Constant + Light.Linear * Dist + Light.Quadratic * (Dist * Dist));
    }

    vec3 HalfwayDir = normalize(LightDir + ViewDir);

    float Diff = max(dot(Normal, LightDir), 0.0);

    vec3 ReflectDir = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(Normal, HalfwayDir), 0.0), 16);

    vec3 LightCol = Light.Color * Light.Intensity;

    vec3 Diffuse  = LightCol * Diff * vec3(texture(GCol, TexCoords).xyz);
    vec3 Specular = LightCol * Spec * vec3(texture(GCol, TexCoords).aaa);

    Diffuse *= Attenuation;
    Specular *= Attenuation;

    return (Diffuse + Specular);
}
