#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

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

	struct graphicsPipelineCreateInfo {
		///////////////////////////////////////////////////////////////////////////
		std::vector<val::shader*> shaders;
		std::vector<VkDynamicState> dynamicStates;
		//std::vector<renderPassInfo*> renderPasses;
		renderPassInfo* renderPassInfo;
		VkPipelineDynamicStateCreateInfo dynamicState{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineDepthStencilStateCreateInfo* depthStencil = NULL;
		//std::vector<VkClearValue> clearValues; // only valid if VK_ATTACHMENT_LOAD_OP_CLEAR is set.

	public:
		std::vector<UBO_Handle*> getUniqueUBOs();

		std::vector<pushConstantHandle*> getUniquePushConstants();

		std::vector<SSBO_Handle*> getUniqueSSBOs();

	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP