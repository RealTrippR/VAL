#ifndef VAL_UBO_HANDLE_HPP
#define VAL_UBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace val {
	struct UBO_Handle {
		UBO_Handle() = default;
		UBO_Handle(uint16_t sizeOfUBO) : UBO_Handle() {
			_size = sizeOfUBO;
		}
		UBO_Handle(uint16_t sizeOfUBO, bufferSpace usage) : UBO_Handle() {
			_size = sizeOfUBO;
			_usage = usage;
		}
	public:
		void update(VAL_PROC& proc, void* data);

		VkBuffer getCurrentBuffer(VAL_PROC& proc);

		std::vector<VkBuffer> getBuffers(VAL_PROC& proc);

		//void* getMappedData(VAL_PROC& pro);

		VkMemoryPropertyFlags getMemoryPropertyFlags();

		static inline VkDescriptorType getVkDescriptorType() { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }

	public:
		uint16_t _size = 0; // Vulkan allows for 16000 as the max size in bytes of a UBO buffer
		int _index = 0;

		VkBufferUsageFlags _additionalUsageFlags = 0;

		// IF GPU ONLY: DOES NOT NEED TO BE MAPPED TO MEMORY
		// IF GPU-CPU: MUST BE MAPPED TO MEMORY
		bufferSpace _usage = CPU_GPU;

		VkShaderStageFlagBits stageFlags = VK_SHADER_STAGE_ALL;
	};
}

#endif // !VAL_UBO_HANDLE_HPP