#version 450

layout(set = 0, binding = 1) uniform View_Matrix {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} viewMatrix;

layout(set = 0, binding = 2) uniform Light_Matrix {
    mat4 space;
    vec3 lightPos;
} lightMatrix;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;              // World-space position
layout(location = 1) out vec3 normal;               // World-space normal
layout(location = 2) out vec4 fragPosLightSpace;    // Light-space position

const mat4 bias = mat4( 
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0,
  0.5, 0.5, 0.0, 1.0 );

void main() {
    vec4 worldPos = viewMatrix.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;

    // Use inverse transpose if your model has non-uniform scaling
    normal = normalize(mat3(transpose(inverse(viewMatrix.model))) * inNormal);

    fragPosLightSpace = bias * lightMatrix.space * worldPos;

    gl_Position = viewMatrix.proj * viewMatrix.view * worldPos;
}
