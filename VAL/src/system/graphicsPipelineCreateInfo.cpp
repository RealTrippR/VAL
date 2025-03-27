#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>

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

	VkPipelineMultisampleStateCreateInfo graphicsPipelineCreateInfo::getVkPipelineMultisampleStateCreateInfo() {
		VkPipelineMultisampleStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.rasterizationSamples = (VkSampleCountFlagBits)_sampleCountMSAA;
		info.sampleShadingEnable = _sampleShadingEnabled;
	}

}