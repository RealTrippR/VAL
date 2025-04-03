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

		renderPassManager* renderPass;

		void setRasterizer(val::rasterizerState* rasterizer);

		rasterizerState* getRasterizer();

		void setColorBlendState(val::colorBlendState* colorState);

		colorBlendState* getColorBlendState();

		void setDepthStencilState(val::depthStencilState* blendState);

		depthStencilState* getDepthStencilState();

		void setSampleCount(const VkSampleCountFlags& samples);

		const VkSampleCountFlags& getSampleCount();

		void setSampleShadingEnabled(const bool& enabled);

		const bool& getSampleShadingEnabled();

		VkPipelineMultisampleStateCreateInfo getVkPipelineMultisampleStateCreateInfo();

		void setTopology(const VkPrimitiveTopology& topolgy);

		VkPrimitiveTopology getTopology();

		void setDynamicStates(const std::vector<DYNAMIC_STATE>& dynamicStates);

		const std::vector<DYNAMIC_STATE>& getDynamicStates();

	public:
		inline VkRenderPass& getVkRenderPass() {
			return renderPass->getVkRenderPass();
		}
	protected:
		friend VAL_PROC;
	protected:
		VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		rasterizerState* _rasterizerState = NULL;
		colorBlendState* _colorBlendState = NULL;

		depthStencilState* _depthStencilState = NULL;

		VkSampleCountFlags _sampleCountMSAA = VK_SAMPLE_COUNT_1_BIT;
		bool _sampleShadingEnabled = false;
		
		// VAL::DYNAMIC_STATE maps directly to VkDynamicState
		std::vector<DYNAMIC_STATE> _dynamicStates;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP