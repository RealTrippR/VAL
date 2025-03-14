#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/pipelineCreateInfo.hpp>

namespace val {
	void pushConstantHandle::update(VAL_PROC& proc, void* data, const pipelineCreateInfo& pipeline, const shader& shdr, VkCommandBuffer& cmdBuffer) {
#ifndef NDEBUG
		if (_size % 4 != 0) {
			printf("VAL: WARNING: Push constant at memory address %h has a size that is not a multiple of 4! It's size is: %d", this, _size);
		}

#endif // !NDEBUG

		vkCmdPushConstants(
			cmdBuffer,
			proc._pipelineLayouts[pipeline.pipelineIdx],
			shdr._shaderStageFlags,
			_offset,
			_size,
			data
		);
	}

	VkPushConstantRange pushConstantHandle::toVkPushConstantRange() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = _stageFlags;  // Used in the vertex shader
		pushConstantRange.offset = _offset;
		pushConstantRange.size = _size;
		return pushConstantRange;
	}
}