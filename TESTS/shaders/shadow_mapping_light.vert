#version 450

layout(set = 0, binding = 0) uniform Light_Matrix {
    mat4 space; // = light projection * light view
    vec3 lightPos;
} lightMatrix;


layout(set = 0, binding = 1) uniform View_Matrix {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
    float farPlane;
} viewMatrix;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 0) out vec4 fragPosLightSpace;



void main() {
    vec4 worldPos = viewMatrix.model * vec4(inPosition, 1.0);
    gl_Position = lightMatrix.space * worldPos;

    // Calculate distance from light position to fragment world position
    //float dist = length(lightMatrix.lightPos - worldPos.xyz);

    // Normalize by far plane
    //fragLinearDepth = dist / lightMatrix.lightFarPlane;
}
