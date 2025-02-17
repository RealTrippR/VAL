#ifndef VAL_PUSH_CONSTANT_HANDLE
#define VAL_PUSH_CONSTANT_HANDLE

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	struct pushConstantHandle {
		pushConstantHandle() = default;
		pushConstantHandle(const uint16_t size, const uint16_t offset = 0) 
		{
			_size = size;
			_offset = offset;
			_procMemoryOffset = 0;
			_stageFlags = 0;
		}
		void update(VAL_PROC& proc, void* data, const uint16_t& pipelineIdx);

		VkPushConstantRange toVkPushConstantRange();
	public:
		uint16_t _size;
		uint16_t _offset;

		VkShaderStageFlags _stageFlags = 0;
		uint16_t _procMemoryOffset = 0;
	};
}

#endif // !PUSH_CONSTANT_HANDLE
