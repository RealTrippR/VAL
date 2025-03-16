#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/system/renderPass.hpp>

namespace val {
	class shader;

	class graphicsPipelineCreateInfo : public pipelineCreateInfo {
	public:		
		std::vector<VkDynamicState> dynamicStates;
		//renderPassInfo* renderPassInfo;
		renderPassManager* renderPass;
		VkPipelineDynamicStateCreateInfo dynamicState{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineDepthStencilStateCreateInfo* depthStencil = NULL;
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP