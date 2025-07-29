#version 450

layout (constant_id = 0) const float BIAS = 0;

const int BLINN = 1;
const int PCF_ITERATIONS = 9;
const int PCF_ITERATIONS_SQ = PCF_ITERATIONS*PCF_ITERATIONS;
const float PCF_SCALE = 1.0;

layout(set = 0, binding = 0) uniform sampler2DShadow shadowMap;

layout(set = 0, binding = 1) uniform View_Matrix {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} viewMatrix;

layout(set = 0,binding = 2) uniform Light_Matrix {
    mat4 lightSpace;
} lightMatrix;

layout(set=0,binding = 3) uniform Light {
    vec3 pos;
    vec3 color;
    float intensity;
} light;
  

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 fragPosLightSpace;

layout(location = 0) out vec4 outColor;

// Thank god for stack overflow
// https://stackoverflow.com/questions/73284425/even-though-the-shadow-map-is-rendered-correctly-shadow-calculations-are-incorr

float ShadowCalculation(vec4 lightSpacePos, vec3 lightDir)
{
    /*
    float shadow = 0.0;
    vec4 shadowCoords = lightSpacePos / lightSpacePos.w;
    if( texture( shadowMap, shadowCoords.xy ).r < shadowCoords.z - BIAS )
    {
        shadow = 1.0;
    }
    return shadow;*/

    vec4 shadowCoords = lightSpacePos / lightSpacePos.w;

    /*
    // early return if no shadow
    if( texture( shadowMap, shadowCoords.xy ).r > shadowCoords.z - BIAS ) {
        return 0.0;
    }
    */
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int PCF_HALF = PCF_ITERATIONS/2;
    const vec2 PFC_TEXEL_SIZE = (texelSize/PCF_HALF) * PCF_SCALE;
    for(int x = -PCF_HALF; x <= PCF_HALF; ++x)
    {
        for(int y = -PCF_HALF; y <= PCF_HALF; ++y)
        {
            /*
            float pcfDepth = texture(shadowMap, shadowCoords.xy + vec2(x, y) * PFC_TEXEL_SIZE).r; 
            shadow += shadowCoords.z - BIAS > pcfDepth ? 1.0 : 0.0;        
            */
            float shadowness = 1-texture(shadowMap, vec3(shadowCoords.xy + vec2(x,y)*PFC_TEXEL_SIZE, shadowCoords.z-BIAS));
            shadow+=shadowness;
        }    
    }
    shadow /= PCF_ITERATIONS_SQ;
    return shadow;
}


void main() { 
    vec3 lightPos = light.pos;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos); // use (-lightDir) for directional

    //a dot product of 0 means vecs are fully perpendicular, 1 is fully parallel.
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewMatrix.viewPos - fragPos);

    float spec;
    if(BLINN == 1)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal,halfwayDir), 0.0), 16.0);
    }
    else
    {
        vec3 reflectDir = reflect(lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
  
    // Phong components
    float shininess = 4;
    float specularStrength = 0.7;

    vec3 ambient = 0.01 * light.color;
    vec3 specular = specularStrength  * light.color * shininess * spec; //* dot(reflectDir,-viewDir);

    float dist = length(lightPos - fragPos);
    float attenuation = 1.0 / (dist * dist);  // inverse square law

    vec3 diffuse = diff * light.color;

    float shadow = ShadowCalculation(fragPosLightSpace,lightDir);
        // Final color with attenuation
    vec3 finalColor = (((ambient + diffuse + specular) * (1.0 - shadow))) * light.intensity * attenuation;
    //finalColor=1-vec3(shadow);
    outColor = vec4(vec3(finalColor), 1.0);
}