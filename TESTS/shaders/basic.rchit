#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitColor;

hitAttributeEXT vec2 attribs;

void main() {
    // barycentrics in attribs.x, attribs.y
    float u = attribs.x;
    float v = attribs.y;

    hitColor = vec3(1.0, 0.0, 0.0); // red
}
