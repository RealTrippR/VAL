#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/system_utils.hpp>

#include <VAL/lib/pipelineStateInfos/stateInfos.hpp>

namespace val {
	class shader;

	class graphicsPipelineCreateInfo : public pipelineCreateInfo {
	public:		
		uint32_t subpassIndex = 0u;

		std::vector<VkDynamicState> dynamicStates; /*WIP*/

		renderPassManager* renderPass;

		void setRasterizer(val::rasterizerState* rasterizer);

		rasterizerState* getRasterizer();

		void setColorBlendState(val::colorBlendState* colorState);

		colorBlendState* getColorBlendState();

		void setSampleCount(const VkSampleCountFlags& samples);

		const VkSampleCountFlags& getSampleCount();

		void setSampleShadingEnabled(const bool& enabled);

		const bool& getSampleShadingEnabled();

		VkPipelineMultisampleStateCreateInfo getVkPipelineMultisampleStateCreateInfo();

		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	public:
		inline VkRenderPass& getVkRenderPass() {
			return renderPass->getVkRenderPass();
		}
	protected:
		friend VAL_PROC;
	protected:
		rasterizerState* _rasterizerState = NULL;
		colorBlendState* _colorBlendState = NULL;

		depthStencilState* depthStencil = NULL;

		VkSampleCountFlags _sampleCountMSAA = VK_SAMPLE_COUNT_1_BIT;
		bool _sampleShadingEnabled = false;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP