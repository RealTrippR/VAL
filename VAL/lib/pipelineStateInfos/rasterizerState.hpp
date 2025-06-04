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
