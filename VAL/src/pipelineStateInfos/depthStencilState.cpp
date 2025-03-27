#include <VAL/lib/pipelineStateInfos/depthStencilState.hpp>

#define stencilState depthStencilState
namespace val
{
	void stencilState::enableDepthTesting(bool enable) {
		_VKdepthState.depthTestEnable = enable;
	}

	void stencilState::enableStencilTesting(bool enable) {
		_VKdepthState.stencilTestEnable = enable;
	}

	void stencilState::enableDepthBoundsTesting(bool enable) {
		_VKdepthState.depthBoundsTestEnable = enable;
	}

	bool stencilState::getDepthBoundsTesting() {
		return _VKdepthState.depthBoundsTestEnable;
	}

	void stencilState::enableStencilTesting(bool enable) {
		_VKdepthState.stencilTestEnable = enable;
	}

	bool stencilState::getStencilTesting() {
		return _VKdepthState.stencilTestEnable;
	}

	void stencilState::setDepthBounds(float lowerBound, float upperBound) {
		_VKdepthState.minDepthBounds = lowerBound;
		_VKdepthState.maxDepthBounds = upperBound;
	}

	void stencilState::setLowerDepthBound(float lowerBound) {
		_VKdepthState.minDepthBounds = lowerBound;
	}

	float stencilState::getLowerDepthBound() {
		return _VKdepthState.minDepthBounds;
	}

	void stencilState::setUpperDepthBound(float upperBound) {
		_VKdepthState.maxDepthBounds = upperBound;
	}

	float stencilState::getUpperDepthBound() {
		return _VKdepthState.maxDepthBounds;
	}


	void stencilState::setFront(const VkStencilOpState& front) {
		_VKdepthState.front = front;
	}

	VkStencilOpState& stencilState::getFront() {
		return _VKdepthState.front;
	}

	void stencilState::setBack(const VkStencilOpState& back) {
		_VKdepthState.back = back;
	}

	VkStencilOpState& stencilState::getBack() {
		return _VKdepthState.back;
	}

	VkPipelineDepthStencilStateCreateInfo& stencilState::getVkPipelineDepthStencilStateCreateInfo() {
		return _VKdepthState;
	}
}