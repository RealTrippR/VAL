#include <VAL/lib/pipelineStateInfos/depthStencilState.hpp>

namespace val
{
	void depthStencilState::enableDepthTesting(bool enable) {
		_VKdepthState.depthTestEnable = enable;
	}

	void depthStencilState::enableStencilTesting(bool enable) {
		_VKdepthState.stencilTestEnable = enable;
	}

	void depthStencilState::enableDepthBoundsTesting(bool enable) {
		_VKdepthState.depthBoundsTestEnable = enable;
	}

	bool depthStencilState::getDepthBoundsTesting() {
		return _VKdepthState.depthBoundsTestEnable;
	}

	bool depthStencilState::getStencilTesting() {
		return _VKdepthState.stencilTestEnable;
	}

	void depthStencilState::setDepthBounds(float lowerBound, float upperBound) {
		_VKdepthState.minDepthBounds = lowerBound;
		_VKdepthState.maxDepthBounds = upperBound;
	}

	void depthStencilState::setLowerDepthBound(float lowerBound) {
		_VKdepthState.minDepthBounds = lowerBound;
	}

	float depthStencilState::getLowerDepthBound() {
		return _VKdepthState.minDepthBounds;
	}

	void depthStencilState::setUpperDepthBound(float upperBound) {
		_VKdepthState.maxDepthBounds = upperBound;
	}

	float depthStencilState::getUpperDepthBound() {
		return _VKdepthState.maxDepthBounds;
	}

	void depthStencilState::setCompareOp(VkCompareOp op) {
		_VKdepthState.depthCompareOp = op;
	}

	VkCompareOp depthStencilState::getCompareOp() {
		return _VKdepthState.depthCompareOp;
	}

	void depthStencilState::setFront(const VkStencilOpState& front) {
		_VKdepthState.front = front;
	}

	VkStencilOpState& depthStencilState::getFront() {
		return _VKdepthState.front;
	}

	void depthStencilState::setBack(const VkStencilOpState& back) {
		_VKdepthState.back = back;
	}

	VkStencilOpState& depthStencilState::getBack() {
		return _VKdepthState.back;
	}

	VkPipelineDepthStencilStateCreateInfo& depthStencilState::getVkPipelineDepthStencilStateCreateInfo() {
		return _VKdepthState;
	}
}