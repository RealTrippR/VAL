/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <vulkan/vulkan_core.h>

#ifndef VAL_VERTEX_ATTRIBUTE_TYPES_HPP
#define VAL_VERTEX_ATTRIBUTE_TYPES_HPP

#include <VAL/lib/ext/tiny_vector.hpp>

namespace val
{
	// this is just a wrapper around VkFormats.
	// It makes them easier to read.
	// Use VERTEX_ATTRIBUTE_TYPE_toVkFormat to convert.
	enum VERTEX_ATTRIBUTE_TYPE
	{
		undefined = VK_FORMAT_UNDEFINED,
		int32 = VK_FORMAT_R32_SINT,
		uint32 = VK_FORMAT_R32_UINT,
		float32 = VK_FORMAT_R32_SFLOAT,
		double64 = VK_FORMAT_R64_SFLOAT,

		ivec2 = VK_FORMAT_R32G32_SINT,
		ivec3 = VK_FORMAT_R32G32B32_SINT,
		ivec4 = VK_FORMAT_R32G32B32A32_SINT,

		uvec2 = VK_FORMAT_R32G32_UINT,
		uvec3 = VK_FORMAT_R32G32B32_UINT,
		uvec4 = VK_FORMAT_R32G32B32A32_UINT,

		vec2 = VK_FORMAT_R32G32_SFLOAT,
		vec3 = VK_FORMAT_R32G32B32_SFLOAT,
		vec4 = VK_FORMAT_R32G32B32A32_SFLOAT,

		dvec2, VK_FORMAT_R64G64_SFLOAT,
		dvec3 = VK_FORMAT_R64G64B64_SFLOAT,
		dvec4 = VK_FORMAT_R64G64B64A64_SFLOAT,


		// these are more complicated because they're split
		// into several vertex input attribute descriptions.
		// for example, a mat2 (2x2 matrix floats) is 
		// equivalent to 4 different vec4 attributes. 
		mat2,
		mat3,
		mat4,

		mat2x2,
		mat3x3,
		mat4x4,

		mat2x3,
		mat3x2,

		mat4x2,
		mat2x4,

		mat4x3,
		mat3x4,
	};

	tiny_vector<VkFormat> VERTEX_ATTRIBUTE_TYPE_toVkFormat(const VERTEX_ATTRIBUTE_TYPE type);
}

#endif // !VAL_VERTEX_ATTRIBUTE_TYPES_HPP
