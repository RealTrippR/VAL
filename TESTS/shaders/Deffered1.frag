layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

void main() {
    gPosition = vec4(fragPos, 1.0);
    gNormal = vec4(normalize(fragNormal), 1.0);
    gAlbedoSpec = vec4(albedoColor.rgb, specularValue);
}
