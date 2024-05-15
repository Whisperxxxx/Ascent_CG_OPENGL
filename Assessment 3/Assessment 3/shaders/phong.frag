#version 450 core

layout (location = 0) out vec4 fColour;

in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace; 
in vec2 tex;

uniform sampler2D Texture;
uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 camPos;

// maximum number of positionallights and spotlights
#define MAX_POSITIONAL_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

// positional light 
struct PositionalLight {
    vec3 position;
    vec3 color;
    float intensity;
};

// spot light
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutOff;
};

uniform int numPositionalLights;
uniform int numSpotLights;
uniform PositionalLight positionalLights[MAX_POSITIONAL_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];


float shadowOnFragment(vec4 FragPosProjectedLightSpace, sampler2D shadowMap, vec3 lightDir)
{
    vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
    vec3 ss = (ndc + 1.0) * 0.5;

    float fragDepth = ss.z;

    float litDepth = texture(shadowMap, ss.xy).r;

    vec3 Nnor = normalize(nor);
    vec3 Ntolight = normalize(-lightDir);
    float bias = max(0.05 * (1.0 - dot(Nnor, Ntolight)), 0.005);

    float shadow = fragDepth > (litDepth + bias) ? 1.0 : 0.0;
    if (fragDepth > 1.0) shadow = 0.0;

    return shadow;
}

vec3 CalculateDirectionalIllumination(vec3 texColor) {

    float k = 0.1;
    vec3 ambient = k * texColor;

    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(-lightDirection);
    float diff = max(dot(Nnor, Nto_light), 0.0);
    vec3 diffuse = diff * texColor;

    vec3 Nfrom_light = normalize(lightDirection);
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float spec = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0); 
    vec3 specular = spec * texColor;

    float shadow = shadowOnFragment(FragPosProjectedLightSpace, shadowMap, lightDirection);
    vec3 phong = ambient + ((1.0 - shadow) * (diffuse + specular));

    return phong * lightColour;
}

vec3 CalculatePositionalIllumination(vec3 texColor, PositionalLight light)
{
    float k = 0.1; 
    vec3 ambient = k * texColor * light.intensity;

    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(light.position - FragPosWorldSpace);
    float diff = max(dot(Nnor, Nto_light), 0.0);
    vec3 diffuse = diff * texColor * light.intensity;

    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float spec = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0); 
    vec3 specular = spec * vec3(1.0f) * light.intensity;

    float distance = length(light.position - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * (distance * distance));
    float shadow = shadowOnFragment(FragPosProjectedLightSpace, shadowMap, Nto_light);

    vec3 phong = (ambient + ((1.f - shadow) * (diffuse + specular))) * attenuation;

    return phong * light.color;
}

vec3 CalculateSpotIllumination(vec3 texColor, SpotLight light)
{
    float k = 0.1;
    vec3 ambient = k * texColor * light.intensity;

    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(light.position - FragPosWorldSpace);
    float diff = max(dot(Nnor, Nto_light), 0.0);
    vec3 diffuse = diff * texColor * light.intensity;

    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirection);
    float spec = pow(max(dot(NcamDirection, NrefLight), 0.0), 128.0);
    vec3 specular = spec * vec3(1.0f) * light.intensity;

    float distance = length(light.position - FragPosWorldSpace);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.002 * (distance * distance));

    float shadow = shadowOnFragment(FragPosProjectedLightSpace, shadowMap, Nto_light);

    float phi = cos(radians(light.cutOff));
    vec3 NSpotDir = normalize(light.direction);
    float theta = dot(Nfrom_light, NSpotDir);

    vec3 phong;
    if (theta > phi)
    {
        phong = (ambient + ((1.f - shadow) * (diffuse + specular))) * attenuation;
    }
    else
    {
        phong = ambient * attenuation;
    }

    return phong * light.color;
}


void main() {
    vec3 texColor = texture(Texture, tex).rgb;
    vec3 phong = CalculateDirectionalIllumination(texColor);

    // add positional illumination from positional lights
    for (int i = 0; i < numPositionalLights; ++i) {
        phong += CalculatePositionalIllumination(texColor, positionalLights[i]);
    }

    // add spot illumination from spot lights
    for (int i = 0; i < numSpotLights; ++i) {
        phong += CalculateSpotIllumination(texColor, spotLights[i]);
    }
    fColour = vec4(phong, 1.0);
}
