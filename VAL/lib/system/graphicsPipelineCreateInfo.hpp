#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/system/renderPass.hpp>

namespace val {
	class shader;

	class graphicsPipelineCreateInfo : public pipelineCreateInfo {
	public:		
		uint32_t subpassIndex = 0u;
		std::vector<VkDynamicState> dynamicStates;
		//renderPassInfo* renderPassInfo;
		renderPassManager* renderPass;
		/*
		VkPipelineDynamicStateCreateInfo dynamicState{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineDepthStencilStateCreateInfo* depthStencil = NULL;
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		*/

		void setRasterizer(val::rasterizerStateInfo* rasterizer);

		rasterizerStateInfo* getRasterizer();

		void setColorBlendState(val::colorBlendState* colorState);

		colorBlendState* getColorBlendState();

		void setMultisamplingLevel(const VkSampleCountFlags& samples);

		const VkSampleCountFlags& getMultisamplingLevel();

		void setSampleShadingEnabled(const bool& enabled);

		const bool& getSampleShadingEnabled();

		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	public:
		inline VkRenderPass& getVkRenderPass() {
			return renderPass->getVkRenderPass();
		}
	protected:
		friend VAL_PROC;
	protected:
		rasterizerStateInfo* _rasterizerState = NULL;
		colorBlendState* _colorBlendState = NULL;

		VkSampleCountFlags _sampleCountMSAA = VK_SAMPLE_COUNT_1_BIT;
		bool _sampleShadingEnabled = false;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP