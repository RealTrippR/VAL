#ifndef VAL_PUSH_CONSTANT_HANDLE_HPP
#define VAL_PUSH_CONSTANT_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class VAL_PROC; // forward declaration
	class pipelineCreateInfo; // forward declaration
	class shader; // forward declaration
	struct pushConstantHandle {
		pushConstantHandle() = default;
		pushConstantHandle(const uint16_t size, const uint16_t offset = 0)
		{
			_size = size;
			_offset = offset;
			_procMemoryOffset = 0;
		}

		void update(VAL_PROC& proc, void* data, const pipelineCreateInfo& pipeline, const shader& shdr, VkCommandBuffer& cmdBuffer);

		VkPushConstantRange toVkPushConstantRange();
	public:
		uint16_t _size;
		uint16_t _offset;

		VkShaderStageFlags _stageFlags = 0;
		uint16_t _procMemoryOffset = 0;
	};
}

#endif // !VAL_PUSH_CONSTANT_HANDLE_HPP