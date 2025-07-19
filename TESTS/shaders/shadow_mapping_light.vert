#version 450

layout(set = 0, binding = 0) uniform LightVP {
    mat4 lightViewProj;
};

layout(location = 0) in vec3 inPosition; // Vertex position in model space

void main() {
    gl_Position = lightViewProj * vec4(inPosition, 1.0);
}
