#version 460
#extension GL_EXT_ray_tracing : require
layout(location = 0) rayPayloadInEXT vec4 payload;
void main() {
    //vec4 outputColor = vec4(0.0, 0.0, 0.0, 1.0);
    //outputColor.r = float(gl_LaunchIDEXT.x) / float(gl_LaunchSizeEXT.x);
    //outputColor.g = float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y);
    payload = vec4(0.7, 0.0, 0.0, 1.0); // red
   // payload = outputColor;
}
