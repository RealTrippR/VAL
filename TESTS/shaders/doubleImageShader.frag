#version 450
#extension GL_ARB_separate_shader_objects : enable
// https://kylehalladay.com/blog/tutorial/vulkan/2018/01/28/Textue-Arrays-Vulkan.html
layout(binding = 1) uniform sampler texSampler;
layout(binding = 2) uniform texture2D texImage[2];  

layout(location = 0) in vec3 fragColor; 
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // Select texture from the array based on some condition (e.g., index or fragment color)
    int textureIndex = 0; // Just an example, choose between 0 and 1
    if (fragColor.r > 0.5) {
        textureIndex = 1;
    }

    outColor = texture(sampler2D(texImage[textureIndex], texSampler), fragTexCoord);
}
