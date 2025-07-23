#version 450

layout (constant_id = 0) const float BIAS = 0;

layout(set = 0, binding = 0) uniform sampler2D  shadowMap;

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
    float shadow = 1.0;
    vec4 shadowCoords = lightSpacePos / lightSpacePos.w;
    if( texture( shadowMap, shadowCoords.xy ).r < shadowCoords.z - BIAS )
    {
        shadow = 0.0;
    }
    return shadow;
    /*
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal);
    float bias = max(0.55 * (1.0 - dot(normal, lightDir)), 0.355);
    bias = BIAS;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    shadow = (currentDepth-bias)>closestDepth  ? 1.0 : 0.0;
    //return shadow;
    return (currentDepth-bias)-closestDepth;*/
}


void main() { 
    vec3 lightPos = light.pos;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos); // use (-lightDir) for directional

    //a dot product of 0 means vecs are fully perpendicular, 1 is fully parallel.
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewMatrix.viewPos - fragPos);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
  
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
    vec3 finalColor = (((ambient + diffuse) * (1.0 - shadow)) + specular) * light.intensity * attenuation;
    //finalColor=1-vec3(shadow);
    finalColor=vec3(shadow);
    outColor = vec4(vec3(finalColor), 1.0);
}