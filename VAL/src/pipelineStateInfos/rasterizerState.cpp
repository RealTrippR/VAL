#include <VAL/lib/pipelineStateInfos/rasterizerStateInfo.hpp>

namespace val
{
	void rasterizerStateInfo::setEnableDepthBias(const bool& enable) {
		_VKrasterizerState.depthBiasEnable = enable;
	}

	const bool& rasterizerStateInfo::getEnableDepthBias() {
		return _VKrasterizerState.depthBiasEnable;
	}

	void rasterizerStateInfo::setConstantDepthBias(const float& bias) {
		_VKrasterizerState.depthBiasConstantFactor = bias;
	}

	const float& rasterizerStateInfo::getConstantDepthBias() {
		return _VKrasterizerState.depthBiasConstantFactor;
	}

	void rasterizerStateInfo::setEnableDepthBiasClamp(const bool& enabled) {
		_VKrasterizerState.depthClampEnable = enabled;
	}

	const bool& rasterizerStateInfo::getEnableDepthBiasClamp() {
		return _VKrasterizerState.depthClampEnable;
	}

	void rasterizerStateInfo::setDepthBiasClamp(const float& clamp) {
		_VKrasterizerState.depthBiasClamp = clamp;
	}

	const float& rasterizerStateInfo::getDepthBiasClamp() {
		return _VKrasterizerState.depthBiasClamp;
	}

	void rasterizerStateInfo::setLineWidth(const float& width, const PIPELINE_PROPERTY_STATE& state) {
		_lineWidthState = state;
		_VKrasterizerState.lineWidth = width;
	}

	const float& rasterizerStateInfo::getLineWidth() {
		return _VKrasterizerState.lineWidth;
	}

	void rasterizerStateInfo::setLineWidthState(const PIPELINE_PROPERTY_STATE& state) {
		_lineWidthState = state;
	}

	const PIPELINE_PROPERTY_STATE& rasterizerStateInfo::getLineWidthState() {
		return _lineWidthState;
	}

	void rasterizerStateInfo::setPolygonMode(const POLYGON_MODE_ENUM& polygonMode) {
		_VKrasterizerState.polygonMode = VkPolygonMode(polygonMode);
	}

	const POLYGON_MODE_ENUM& rasterizerStateInfo::getPolygonMode() {
		return (POLYGON_MODE_ENUM)_VKrasterizerState.polygonMode;
	}

	const CULL_MODE_ENUMS& rasterizerStateInfo::getCullMode() {
		return CULL_MODE_ENUMS(_VKrasterizerState.cullMode);
	}

	void rasterizerStateInfo::setCullMode(const CULL_MODE_ENUMS& cullMode) {
		_VKrasterizerState.cullMode = VkCullModeFlags(cullMode);
	}
}