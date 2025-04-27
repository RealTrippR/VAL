#ifndef VAL_UBO_HANDLE_HPP
#define VAL_UBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

#include <VAL/lib/system/UBO_arr_manager.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace val {
	struct UBO_Handle {
		UBO_Handle(uint16_t sizeOfUBO, bufferSpace space = CPU_GPU) : _size(sizeOfUBO), _space(space) {};
		UBO_Handle(uint16_t sizeOfUBO, bufferSpace space, VkBufferUsageFlags additionalUsageFlags) : _size(sizeOfUBO), _space(space), _additionalUsageFlags(additionalUsageFlags){};

	public:
		void* getData(VAL_PROC& proc);

		void* getData(VAL_PROC& proc, const uint8_t frameIdx);

		VkBuffer getBuffer(VAL_PROC& proc);

		void update(VAL_PROC& proc, void* data);

		void update(VAL_PROC& proc, void* data, const uint8_t frameIdx);

		uboArraySubset* getUBOarraySubset(VAL_PROC& proc);


		//void* getMappedData(VAL_PROC& pro);

		VkMemoryPropertyFlags getMemoryPropertyFlags();

		static inline VkDescriptorType getVkDescriptorType() { return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }

	public:
		const uint32_t _size = 0u; // Vulkan spec states that every device must support at least 16KB as the max size in bytes of a UBO buffer, some go up to 64 KB in size
		uint32_t _offset = 0u;

		uboArraySubset* _arrSubset = NULL;

		const VkBufferUsageFlags _additionalUsageFlags = 0;

		// IF GPU ONLY: DOES NOT NEED TO BE MAPPED TO MEMORY
		// IF GPU-CPU: MUST BE MAPPED TO MEMORY
		const bufferSpace _space;

		VkShaderStageFlagBits stageFlags = VK_SHADER_STAGE_ALL;
	};
}

#endif // !VAL_UBO_HANDLE_HPP