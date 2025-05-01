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

#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>
#include <VAL/lib/pipelineStateInfos/stateInfos.hpp>

#include <stdio.h>


namespace val
{

	void graphicsPipelineCreateInfo::setRasterizer(val::rasterizerState* rasterizer)
	{
		_rasterizerState = rasterizer;
	}

	rasterizerState* graphicsPipelineCreateInfo::getRasterizer()
	{
		return _rasterizerState;
	}

	void graphicsPipelineCreateInfo::setColorBlendState(val::colorBlendState* colorState) {
		_colorBlendState = colorState;
	}

	colorBlendState* graphicsPipelineCreateInfo::getColorBlendState() {
		return _colorBlendState;
	}

	void graphicsPipelineCreateInfo::setDepthStencilState(val::depthStencilState* blendState) {
		_depthStencilState = blendState;
	}

	depthStencilState* graphicsPipelineCreateInfo::getDepthStencilState() {
		return _depthStencilState;
	}

	void graphicsPipelineCreateInfo::setSampleCount(const VkSampleCountFlags& samples)
	{
		_sampleCountMSAA = samples;
	}

	const VkSampleCountFlags& graphicsPipelineCreateInfo::getSampleCount()
	{
		return _sampleCountMSAA;
	}

	void graphicsPipelineCreateInfo::setSampleShadingEnabled(const bool& enabled) {
		_sampleShadingEnabled = enabled;
	}

	const bool& graphicsPipelineCreateInfo::getSampleShadingEnabled() {
		return _sampleShadingEnabled;
	}

	VkPipelineMultisampleStateCreateInfo graphicsPipelineCreateInfo::getVkPipelineMultisampleStateCreateInfo() {
		VkPipelineMultisampleStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.rasterizationSamples = (VkSampleCountFlagBits)_sampleCountMSAA;
		info.sampleShadingEnable = _sampleShadingEnabled;
		return info;
	}

	void graphicsPipelineCreateInfo::setTopology(const VkPrimitiveTopology& topolgy) {
		_topology = topolgy;
	}

	VkPrimitiveTopology graphicsPipelineCreateInfo::getTopology() {
		return _topology;
	}

	void graphicsPipelineCreateInfo::setDynamicStates(const std::vector<DYNAMIC_STATE>& dynamicStates) {
		_dynamicStates = dynamicStates;
	}

	const std::vector<DYNAMIC_STATE>& graphicsPipelineCreateInfo::getDynamicStates() {
		return _dynamicStates;
	}
}