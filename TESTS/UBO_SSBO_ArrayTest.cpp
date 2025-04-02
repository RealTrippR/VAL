/*
* In Vulkan, SSBOs and UBOs can be binded as an array. This is not yet a supported feature in VAL,
* but there are plans to implement it as a feature.
*
* 
* 
layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
} ubos[BUFFER_ARRAY_SIZE];

void main() {
    mat4 modelMatrix = ubos[gl_InstanceIndex].model;
}
*
* 
*/