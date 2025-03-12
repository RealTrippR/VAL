#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/pipelineCreateInfo.hpp>

namespace val {
	void pushConstantHandle::update(VAL_PROC& proc, void* data, const pipelineCreateInfo& pipeline, VkCommandBuffer cmdBuffer) {

		vkCmdPushConstants(
			cmdBuffer,
			proc._pipelineLayouts[pipeline.pipelineIdx],
			_stageFlags,
			_offset,
			_size,
			data
		);

		memcpy(((char*)proc._pushConstantData) + _procMemoryOffset, data, _size);
	}

	void pushConstantHandle::update(VAL_PROC& proc, void* data, const pipelineCreateInfo& pipeline) {

		VkCommandBuffer cmdBuffer = proc.beginSingleTimeCommands();
		vkCmdPushConstants(
			cmdBuffer,
			proc._pipelineLayouts[pipeline.pipelineIdx],
			_stageFlags,
			_offset,
			_size,
			data
		);

		proc.endSingleTimeCommands(cmdBuffer);
		memcpy(((char*)proc._pushConstantData) + _procMemoryOffset, data, _size);
	}

	VkPushConstantRange pushConstantHandle::toVkPushConstantRange() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = _stageFlags;  // Used in the vertex shader
		pushConstantRange.offset = _offset;
		pushConstantRange.size = _size;
		return pushConstantRange;
	}
}