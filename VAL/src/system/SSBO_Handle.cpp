#include <VAL/lib/system/SSBO_Handle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void SSBO_Handle::update(VAL_PROC& proc, void* data) {
#ifndef NDEBUG
		if (_usage != CPU_GPU) {
			printf("VAL: A BUFFER WITH A USAGE BIT OF CPU_GPU CANNOT BE WRITTEN TO OR READ BY THE CPU!\n");
			throw std::runtime_error("VAL: A BUFFER WITH A USAGE BIT OF CPU_GPU CANNOT BE WRITTEN TO OR READ BY THE CPU!");
		}
#endif // !NDEBUG

		memcpy(proc._SSBO_DataMapped[proc._currentFrame][_index], data, _size);
	}

	void SSBO_Handle::updateFromTempStagingBuffer(VAL_PROC& proc, void* data) {
		// Create a staging buffer used to upload data to the gpu
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		proc.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		vkMapMemory(proc._device, stagingBufferMemory, 0, _size, 0, &data);
		memcpy(data, data, _size);
		vkUnmapMemory(proc._device, stagingBufferMemory);
		
		// Copy initial particle data to all storage buffers
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; i++) {
			proc.copyBuffer(stagingBuffer, proc._SSBO_Buffers[_index][0], _size);
		}

		vkDestroyBuffer(proc._device, stagingBuffer, nullptr);
		vkFreeMemory(proc._device, stagingBufferMemory, nullptr);
	}

	void SSBO_Handle::updateFromStagingBuffer(VAL_PROC& proc, VkBuffer buffer) {
		// Copy initial particle data to all storage buffers
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; i++) {
			proc.copyBuffer(buffer, proc._SSBO_Buffers[_index][0], _size);
		}
	}

	void SSBO_Handle::resize(VAL_PROC& proc, size_t size) {
		throw std::runtime_error("VAL: SSBO_HANDLE::RESIZE IS AN INCOMPLETE FUNCTION");
	}

	VkBuffer SSBO_Handle::getBuffer(VAL_PROC& proc) {
		return proc._SSBO_Buffers[proc._currentFrame][_index];
	}

	VkMemoryPropertyFlagBits SSBO_Handle::getMemoryPropertyFlagBits() {
		switch (_usage) {
		case GPU_ONLY:
			return VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		case CPU_GPU:
			return VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			// this can be optimized, some GPUs support VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			// it would be smart to use this memory property whenever supported.
		}
	}
}