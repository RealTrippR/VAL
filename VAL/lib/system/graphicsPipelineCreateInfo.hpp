#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

#include <VAL/lib/system/pipelineCreateInfo.hpp>

namespace val {
	class shader;

	class graphicsPipelineCreateInfo : public pipelineCreateInfo {
	public:
		std::vector<shader*> shaders;
		
		std::vector<VkDynamicState> dynamicStates;
		renderPassInfo* renderPassInfo;
		VkPipelineDynamicStateCreateInfo dynamicState{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineDepthStencilStateCreateInfo* depthStencil = NULL;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP