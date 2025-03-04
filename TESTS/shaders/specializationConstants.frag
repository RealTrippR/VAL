#version 450

layout(location=0) in vec3 fragColor;
layout( constant_id = 1 ) const float R_OFFSET = .5;  // Specialization constant
layout(location=0) out vec4 outColor;

void main()
{
	outColor = vec4(fragColor,1.0);
	outColor.x += R_OFFSET.x;
}