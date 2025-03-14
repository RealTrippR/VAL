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
		memcpy(proc._SSBO_DataMapped[proc._currentFrame][_index],data,_size);
	}

	std::vector<VkBuffer> SSBO_Handle::getBuffers(VAL_PROC& proc) {
		std::vector<VkBuffer> buffers;
		for (uint_fast8_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			buffers.push_back(proc._SSBO_Buffers[i][_index]);
		}
		return buffers;
	}

	void SSBO_Handle::updateFromTempStagingBuffer(VAL_PROC& proc, void* data) {
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		proc.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		// Map memory and copy data into the staging buffer
		void* mappedData;
		if (vkMapMemory(proc._device, stagingBufferMemory, 0, _size, 0, &mappedData) != VK_SUCCESS) {
			throw std::runtime_error("Failed to map staging buffer memory.");
		}
		memcpy(mappedData, data, _size);
		vkUnmapMemory(proc._device, stagingBufferMemory);

		// Copy data from staging buffer to all storage buffers for each frame
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; i++) {
			proc.copyBuffer(stagingBuffer, getBuffers(proc)[i], _size);
		}

		vkDestroyBuffer(proc._device, stagingBuffer, nullptr);
		vkFreeMemory(proc._device, stagingBufferMemory, nullptr);
	}


	VkMemoryPropertyFlags SSBO_Handle::getMemoryPropertyFlags() {
		return bufferSpaceToVkMemoryProperty(_usage);
	}
}