#version 450

layout(set = 0, binding = 0) uniform sampler2DShadow shadowMap;

layout(set = 0, binding = 1) uniform View_Matrix {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} viewMatrix;

layout(set = 0,binding = 2) uniform Light_Matrix {
    mat4 lightViewProj;
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

float calculateShadow(vec4 fragLightSpace) {
    // Transform light-space position to NDC
    vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;

    // Transform to [0, 1] UV space
    projCoords = projCoords * 0.5 + 0.5;

    // Optional: discard if outside shadow map range
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    // Bias to prevent shadow acne
    float bias = 0.005;

    // PCF (simple 3x3 filter)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            shadow += texture(shadowMap, vec3(projCoords.xy + offset, projCoords.z - bias));
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(light.pos - fragPos);
    vec3 V = normalize(viewMatrix.viewPos - fragPos);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 32.0);

    vec3 ambient = 0.1 * vec3(1.0);
    vec3 diffuse = diff * vec3(1.0);
    vec3 specular = spec * vec3(1.0);

    float shadow = calculateShadow(fragPosLightSpace);

    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    outColor = vec4(lighting, 1.0);
}
