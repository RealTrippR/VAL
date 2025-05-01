/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;
	};
}

#endif // !VAL_UBO_HANDLE_HPP