#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitColor;

hitAttributeEXT vec2 attribs;

void main() {
    // barycentrics in attribs.x, attribs.y
    float u = attribs.x;
    float v = attribs.y;

    // Interpolate texture coords, normals, colors, etc. here using u, v

    //hitColor = vec3(u, v, 0.0);  // Just example usage

    hitColor = vec3(1.0, 0.0, 0.0); // Flat red for hit
}
