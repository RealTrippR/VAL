#version 450
#extension GL_ARB_separate_shader_objects : enable
// https://kylehalladay.com/blog/tutorial/vulkan/2018/01/28/Textue-Arrays-Vulkan.html
layout(binding = 1) uniform sampler texSampler;
//layout(binding = 2) uniform texture2D texImage[2];
// the set index must be specified, in VAL the set index for push descriptors is always 1
layout(set = 1, binding = 2) uniform texture2D texImage[2];  

layout(location = 0) in vec3 fragColor; 
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform constants
{
	float scissor;
} pushConstant;

void main() {
    int textureIndex = 0;
    if (fragColor.r > pushConstant.scissor) { // the r value is the x cord in a range from 0-1
        textureIndex = 1;
    }

    outColor = texture(sampler2D(texImage[textureIndex], texSampler), fragTexCoord);
}
