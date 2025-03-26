#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>

namespace val
{
	void graphicsPipelineCreateInfo::setRasterizer(val::rasterizerStateInfo* rasterizer)
	{
		_rasterizerState = rasterizer;
	}

	rasterizerStateInfo* graphicsPipelineCreateInfo::getRasterizer()
	{
		return _rasterizerState;
	}

	void graphicsPipelineCreateInfo::setColorBlendState(val::colorBlendState* colorState) {
		_colorBlendState = colorState;
	}

	colorBlendState* graphicsPipelineCreateInfo::getColorBlendState() {
		return _colorBlendState;
	}

	void graphicsPipelineCreateInfo::setMultisamplingLevel(const VkSampleCountFlags& samples)
	{
		_sampleCountMSAA = samples;
	}

	const VkSampleCountFlags& graphicsPipelineCreateInfo::getMultisamplingLevel()
	{
		return _sampleCountMSAA;
	}

	void graphicsPipelineCreateInfo::setSampleShadingEnabled(const bool& enabled) {
		_sampleShadingEnabled = enabled;
	}

	const bool& graphicsPipelineCreateInfo::getSampleShadingEnabled() {
		return _sampleShadingEnabled;
	}
}