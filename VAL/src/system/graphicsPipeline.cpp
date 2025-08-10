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

#include <VAL/lib/system/VAL_PROC.hpp>

#include <VAL/lib/system/graphicsPipeline.hpp>
#include <VAL/lib/pipelineStateInfos/stateInfos.hpp>

#include <stdio.h>



namespace val
{
	inline std::unordered_map<val::GraphicsPipeline*, tiny_vector<VkRect2D>>& pipelineScissors()
	{
		static std::unordered_map<val::GraphicsPipeline*, tiny_vector<VkRect2D>> map;
		return map;
	}

	inline std::unordered_map<val::GraphicsPipeline*, tiny_vector<VkViewport>>& pipelineViewports()
	{
		static std::unordered_map<val::GraphicsPipeline*, tiny_vector<VkViewport>> map;
		return map;
	}


	void GraphicsPipeline::setRasterizer(val::rasterizerState* rasterizer)
	{
		_rasterizerState = rasterizer;
	}

	rasterizerState* GraphicsPipeline::getRasterizer() const
	{
		return _rasterizerState;
	}

	void GraphicsPipeline::setColorBlendState(val::ColorBlendState* colorState) {
		_colorBlendState = colorState;
	}

	ColorBlendState* GraphicsPipeline::getColorBlendState() const {
		return _colorBlendState;
	}

	void GraphicsPipeline::setDepthStencilState(val::depthStencilState* blendState) {
		_depthStencilState = blendState;
	}

	depthStencilState* GraphicsPipeline::getDepthStencilState() const {
		return _depthStencilState;
	}

	void GraphicsPipeline::setSampleCount(const VkSampleCountFlags& samples)
	{
		_sampleCountMSAA = samples;
	}

	const VkSampleCountFlags& GraphicsPipeline::getSampleCount() const
	{
		return _sampleCountMSAA;
	}

	void GraphicsPipeline::setSampleShadingEnabled(const bool& enabled) 
	{
		_sampleShadingEnabled = enabled;
	}

	const bool& GraphicsPipeline::getSampleShadingEnabled() const 
	{
		return _sampleShadingEnabled;
	}

	VkPipelineMultisampleStateCreateInfo GraphicsPipeline::getVkPipelineMultisampleStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.rasterizationSamples = (VkSampleCountFlagBits)_sampleCountMSAA;
		info.sampleShadingEnable = _sampleShadingEnabled;
		return info;
	}

	void GraphicsPipeline::setTopology(const VkPrimitiveTopology& topolgy) 
	{
		_topology = topolgy;
	}

	VkPrimitiveTopology GraphicsPipeline::getTopology() const {
		return _topology;
	}



	void GraphicsPipeline::setScissors(const tiny_vector<VkRect2D> scissors)
	{
		pipelineScissors()[this];
		pipelineScissors()[this] = scissors;
	}

	void GraphicsPipeline::setScissorCount(const uint32_t count) 
	{
		pipelineScissors()[this].resize(count);
	}


	void GraphicsPipeline::setViewports(const tiny_vector<VkViewport> viewport)
	{
		pipelineViewports()[this] = viewport;
	}

	void GraphicsPipeline::setViewportCount(const uint32_t count)
	{
		pipelineViewports()[this].resize(count);
	}

	std::optional<tiny_vector<VkRect2D>> GraphicsPipeline::getScissors() const
	{
		if (pipelineScissors().count(const_cast<GraphicsPipeline*>(this)) > 0) {
			return pipelineScissors()[const_cast<GraphicsPipeline*>(this)];
		}
		return std::nullopt;
	}

	uint32_t GraphicsPipeline::getScissorCount() const
	{
		if (pipelineScissors().count(const_cast<GraphicsPipeline*>(this)) > 0) {
			return pipelineScissors()[const_cast<GraphicsPipeline*>(this)].size();
		}
		return 0;
	}


	std::optional<tiny_vector<VkViewport>> GraphicsPipeline::getViewports() const
	{
		if (pipelineViewports().count(const_cast<GraphicsPipeline*>(this)) > 0) {
			return pipelineViewports()[const_cast<GraphicsPipeline*>(this)];
		}
		return std::nullopt;
	}

	uint32_t GraphicsPipeline::getViewportCount() const
	{
		if (pipelineViewports().count(const_cast<GraphicsPipeline*>(this)) > 0) {
			return pipelineViewports()[const_cast<GraphicsPipeline*>(this)].size();
		}
		return 0;
	}

}