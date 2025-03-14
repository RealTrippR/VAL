#ifndef FML_PIPELINE_CREATE_INFO_HPP
#define FML_PIPELINE_CREATE_INFO_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/renderPassInfo.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>
#include <vector>
#include <algorithm>

namespace val {
	class shader;

	class pipelineCreateInfo {
	public:
		std::vector<shader*> shaders;
		uint32_t pipelineIdx = 0u;
		uint32_t descriptorsIdx = 0u; // index of descriptor sets and layouts
	public:
		virtual std::vector<UBO_Handle*> getUniqueUBOs();

		virtual std::vector<pushConstantHandle*> getUniquePushConstants();

		virtual std::vector<SSBO_Handle*> getUniqueSSBOs();

		virtual std::vector<VkDescriptorSet> getDescriptorSets(VAL_PROC& proc);

		//std::vector<VkPipelineStageFlags> getPipelineStages();

		virtual const std::vector<VkShaderStageFlags> getShaderStages();

	};
}

#endif // !FML_COMPUTE_PIPELINE_CREATE_INFO_HPP