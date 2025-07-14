#version 460

layout(binding = 0) uniform ViewUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} view_info;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragViewPos;

void main() 
{
    gl_Position = view_info.proj * view_info.view * view_info.model * vec4(inPosition, 1.0);

    vec4 worldPos = view_info.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = mat3(transpose(inverse(view_info.model))) * inNormal;
    fragTexCoord = inTexCoord;
}
