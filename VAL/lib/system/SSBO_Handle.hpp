#ifndef VAL_SSBO_HANDLE_HPP
#define VAL_SSBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace val {
	class SSBO_Handle {
	public:
		SSBO_Handle() = default;
		SSBO_Handle(uint64_t sizeOfSSBO, bufferUsage usage = CPU_GPU) {
			_size = sizeOfSSBO;
			_usage = usage;
		}
	public:
		void update(VAL_PROC& proc, void* data);

		void updateFromTempStagingBuffer(VAL_PROC& proc, void* data);

		void updateFromStagingBuffer(VAL_PROC& proc, VkBuffer buffer);

		void resize(VAL_PROC& proc, size_t size);

		//void* getMappedData(VAL_PROC& pro);

		VkBuffer getBuffer(VAL_PROC& proc);

		VkMemoryPropertyFlagBits getMemoryPropertyFlagBits();
	public:
		uint64_t _size = 0;
		int _index = 0;

		VkBufferUsageFlags _additionalUsageFlags;

		// IF GPU ONLY: DOES NOT NEED TO BE MAPPED TO MEMORY
		// IF GPU-CPU: MUST BE MAPPED TO MEMORY
		bufferUsage _usage = CPU_GPU;

		VkShaderStageFlags _stageFlags = 0;
	};
}

#endif // !VAL_SSBO_HANDLE_HPP