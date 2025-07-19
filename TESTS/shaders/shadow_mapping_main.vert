#version 450

layout(set = 0, binding = 1) uniform View_Matrix {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} viewMatrix;

layout(set = 0, binding = 2) uniform Light_Matrix {
    mat4 lightViewProj;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;              // World-space position
layout(location = 1) out vec3 normal;               // World-space normal
layout(location = 2) out vec4 fragPosLightSpace;    // Light-space position

void main() {
    vec4 worldPos = viewMatrix.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;

    // Normal in world space
    normal = normalize(mat3(viewMatrix.model) * inNormal);

    // Transform to light space for shadow lookup
    fragPosLightSpace = lightViewProj * worldPos;

    // Final screen position
    gl_Position = viewMatrix.proj * viewMatrix.view * worldPos;
}
