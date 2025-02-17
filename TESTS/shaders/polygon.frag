//https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
#version 450

layout(location=0) in vec3 fragColor;

layout(location=0) out vec4 outColor;

void main()
{
	outColor = vec4(fragColor,1.0);
}