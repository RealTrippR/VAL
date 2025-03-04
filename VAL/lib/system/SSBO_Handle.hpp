#ifndef VAL_SSBO_HANDLE_HPP
#define VAL_SSBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace val {
	class SSBO_Handle {
	public:
		SSBO_Handle() = default;
		SSBO_Handle(uint64_t sizeOfSSBO) : SSBO_Handle() {
			_size = sizeOfSSBO;
		}
		SSBO_Handle(uint64_t sizeOfSSBO, bufferSpace usage) : SSBO_Handle() {
			_size = sizeOfSSBO;
			_usage = usage;
		}
	public:
		void update(VAL_PROC& proc, void* data);

		void updateFromTempStagingBuffer(VAL_PROC& proc, void* data);

		//void resize(VAL_PROC& proc, size_t size);

		//void* getMappedData(VAL_PROC& pro);

		std::vector<VkBuffer> getBuffers(VAL_PROC& proc);

		VkMemoryPropertyFlags getMemoryPropertyFlags();
	public:
		uint64_t _size = 0;
		int _index = 0;

		VkBufferUsageFlags _additionalUsageFlags = 0;

		// IF GPU ONLY: DOES NOT NEED TO BE MAPPED TO MEMORY
		// IF GPU-CPU: MUST BE MAPPED TO MEMORY
		bufferSpace _usage = CPU_GPU;

		VkShaderStageFlags _stageFlags = VK_SHADER_STAGE_ALL;
	};
}

#endif // !VAL_SSBO_HANDLE_HPP