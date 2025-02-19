#ifndef VAL_UBO_HANDLE_HPP
#define VAL_UBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace val {
	struct UBO_Handle {
		UBO_Handle() = default;
		UBO_Handle(uint16_t sizeOfUBO) {
			_size = sizeOfUBO;
		}
	public:
		void update(VAL_PROC& proc, void* data);

		VkBuffer getBuffer(VAL_PROC& proc);

		//void* getMappedData(VAL_PROC& pro);

		VkMemoryPropertyFlagBits getMemoryPropertyFlagBits();

	public:
		uint16_t _size = 0; // Vulkan allows for 16000 as the max size in bytes of a UBO buffer
		int _index = 0;

		VkBufferUsageFlags _additionalUsageFlags;

		// IF GPU ONLY: DOES NOT NEED TO BE MAPPED TO MEMORY
		// IF GPU-CPU: MUST BE MAPPED TO MEMORY
		bufferUsage _usage = CPU_GPU;

		VkShaderStageFlagBits stageFlags;
	};
}

#endif // !VAL_UBO_HANDLE_HPP