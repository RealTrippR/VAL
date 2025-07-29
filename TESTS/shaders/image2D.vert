#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;


layout(location = 0) out vec2 fragPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 0.0, 1.0);
    fragTexCoord = texCoord;
    fragPos = pos;
    fragNormal = mat3(transpose(inverse(ubo.model))) * normal;
}