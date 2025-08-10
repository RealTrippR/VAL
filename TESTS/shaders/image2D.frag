#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform constants
{
	bool addRed;
} pushContants;

void main() {
	if (pushContants.addRed==true) {
		outColor = texture(texSampler, texCoord) + vec4(.5,0,0,0);
	} else {
		outColor = texture(texSampler, texCoord);
	}
}