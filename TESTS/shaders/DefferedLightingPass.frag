layout(set = 0, binding = 0) uniform sampler2D gPosition;
layout(set = 0, binding = 1) uniform sampler2D gNormal;
layout(set = 0, binding = 2) uniform sampler2D gAlbedoSpec;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 pos = texture(gPosition, uv).xyz;
    vec3 normal = normalize(texture(gNormal, uv).xyz);
    vec3 albedo = texture(gAlbedoSpec, uv).rgb;
    float spec = texture(gAlbedoSpec, uv).a;

    // Perform lighting calculations here
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;

    // ... add specular, ambient, etc.

    outColor = vec4(diffuse, 1.0);
}
