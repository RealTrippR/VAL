#include <VAL/lib/pipelineStateInfos/rasterizerState.hpp>

namespace val
{
	void rasterizerState::setEnableDepthBias(const bool& enable) {
		_VKrasterizerState.depthBiasEnable = enable;
	}

	const bool& rasterizerState::getEnableDepthBias() {
		return _VKrasterizerState.depthBiasEnable;
	}

	void rasterizerState::setConstantDepthBias(const float& bias) {
		_VKrasterizerState.depthBiasConstantFactor = bias;
	}

	const float& rasterizerState::getConstantDepthBias() {
		return _VKrasterizerState.depthBiasConstantFactor;
	}

	void rasterizerState::setEnableDepthBiasClamp(const bool& enabled) {
		_VKrasterizerState.depthClampEnable = enabled;
	}

	const bool& rasterizerState::getEnableDepthBiasClamp() {
		return _VKrasterizerState.depthClampEnable;
	}

	void rasterizerState::setDepthBiasClamp(const float& clamp) {
		_VKrasterizerState.depthBiasClamp = clamp;
	}

	const float& rasterizerState::getDepthBiasClamp() {
		return _VKrasterizerState.depthBiasClamp;
	}

	void rasterizerState::setLineWidth(const float& width, const PIPELINE_PROPERTY_STATE& state) {
		_lineWidthState = state;
		_VKrasterizerState.lineWidth = width;
	}

	const float& rasterizerState::getLineWidth() {
		return _VKrasterizerState.lineWidth;
	}

	void rasterizerState::setLineWidthState(const PIPELINE_PROPERTY_STATE& state) {
		_lineWidthState = state;
	}

	const PIPELINE_PROPERTY_STATE& rasterizerState::getLineWidthState() {
		return _lineWidthState;
	}

	void rasterizerState::setTopologyMode(const TOPOLOGY_MODE& polygonMode) {
		_VKrasterizerState.polygonMode = VkPolygonMode(polygonMode);
	}

	const TOPOLOGY_MODE& rasterizerState::getPolygonMode() {
		return (TOPOLOGY_MODE)_VKrasterizerState.polygonMode;
	}

	const CULL_MODE& rasterizerState::getCullMode() {
		return CULL_MODE(_VKrasterizerState.cullMode);
	}

	void rasterizerState::setCullMode(const CULL_MODE& cullMode) {
		_VKrasterizerState.cullMode = VkCullModeFlags(cullMode);
	}

	VkPipelineRasterizationStateCreateInfo* rasterizerState::getVkPipelineRasterizationStateCreateInfo() {
		return &_VKrasterizerState;
	}
}