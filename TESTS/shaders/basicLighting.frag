#version 450

layout(location=0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform Light {
    vec3 lightPos;
    vec3 lightColor;
} light;

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.lightPos - fragPos); // use (-lightDir) for directional
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = 0.1 * light.lightColor;
    vec3 diffuse = diff * light.lightColor;

    vec3 finalColor = ambient + diffuse;

    outColor = vec4(fragColor + finalColor, 1.0);
}