#ifndef VAL_RASTERIZER_STATE_INFO_HPP
#define VAL_RASTERIZER_STATE_INFO_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class rasterizerState
	{
	public:

		void setEnableDepthBias(const bool& enable);

		const bool& getEnableDepthBias();

		void setConstantDepthBias(const float& bias);

		const float& getConstantDepthBias();

		void setEnableDepthBiasClamp(const bool& enabled);

		const bool& getEnableDepthBiasClamp();
		
		void setDepthBiasClamp(const float& clamp);

		const float& getDepthBiasClamp();

		void setLineWidth(const float& width, const PIPELINE_PROPERTY_STATE& state = PIPELINE_PROPERTY_STATE::STATIC);

		const float& getLineWidth();

		void setLineWidthState(const PIPELINE_PROPERTY_STATE& state);

		const PIPELINE_PROPERTY_STATE& getLineWidthState();

		void setTopologyMode(const TOPOLOGY_MODE& polygonMode);

		const TOPOLOGY_MODE& getPolygonMode();

		void setCullMode(const CULL_MODE& cullMode);

		const CULL_MODE& getCullMode();

		VkPipelineRasterizationStateCreateInfo* getVkPipelineRasterizationStateCreateInfo();

	protected:
		friend class VAL_PROC;

		PIPELINE_PROPERTY_STATE _lineWidthState = PIPELINE_PROPERTY_STATE::STATIC;
		VkPipelineRasterizationStateCreateInfo _VKrasterizerState
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f
		};
	};
}

#endif // !VAL_RASTERIZER_STATE_INFO_HPP
