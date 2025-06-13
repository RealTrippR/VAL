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

#ifndef SHADER_STAGE_ENUM_HPP
#define SHADER_STAGE_ENUM_HPP



#include <VAL/lib/classEnumBitOps.hpp>
#include <ExternalLibraries/Vulkan/Include/vulkan/vulkan_core.h>

namespace val {

	enum class SHADER_STAGE : uint32_t {
		Vertex = VK_SHADER_STAGE_VERTEX_BIT,
		TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
		TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
		Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
		Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
		Compute = VK_SHADER_STAGE_COMPUTE_BIT,
		AllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS,
		All = VK_SHADER_STAGE_ALL,
		Raygen = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
		AnyHit = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
		ClosestHit = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
		Miss = VK_SHADER_STAGE_MISS_BIT_KHR,
		Intersection = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
		Callable = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
		Task = VK_SHADER_STAGE_TASK_BIT_EXT,
		Mesh = VK_SHADER_STAGE_MESH_BIT_EXT
	};

	inline bool operator==(const VkShaderStageFlags& a, const SHADER_STAGE& b) {
		return bool(a == b); 
	}
	inline bool operator==(const VkShaderStageFlagBits& a, const SHADER_STAGE& b) {
		return bool(a == b);
	}

	inline bool operator&(const VkShaderStageFlags& a, const SHADER_STAGE& b) {
		return bool(a & b);
	}
	inline bool operator&(const VkShaderStageFlagBits& a, const SHADER_STAGE& b) {
		return bool(a & b);
	}
	

	inline bool operator|(const VkShaderStageFlags& a, const SHADER_STAGE& b) {
		return bool(a | b);
	}
	inline bool operator|(const VkShaderStageFlagBits& a, const SHADER_STAGE& b) {
		return bool(a | b);
	}
	
	inline bool operator^(const VkShaderStageFlags& a, const SHADER_STAGE& b) {
		return bool(a ^ b);
	}
	inline bool operator^(const VkShaderStageFlagBits& a, const SHADER_STAGE& b) {
		return bool(a ^ b);
	}

#ifndef SHADER_STAGE_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS
#define SHADER_STAGE_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS

	DEF_ENUM_BITWISE_OPERATORS(SHADER_STAGE);
#endif
}

#endif // !SHADER_STAGE_ENUM_HPP