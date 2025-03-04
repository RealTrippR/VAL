#include <VAL/lib/system/buffer.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void buffer::create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& space, VkBufferUsageFlags bufferUsage) {
		proc.createBuffer(size, bufferUsage, bufferSpaceToVkMemoryProperty(space), _buffer, _memory);
		_size = size;
		_space = space;
		_usage = bufferUsage;

		// create mapped data memory
		if (CPU_GPU == space) {
			_dataMapped = malloc(size);
			vkMapMemory(proc._device, _memory, 0u, size, 0u, &_dataMapped);
		}
	}

	void buffer::overwriteFromStagingBuffer(VAL_PROC& proc, void* data, uint32_t dataSize) {
#ifndef NDEBUG
		if (dataSize > _size) {
			printf("VAL: WARNING: the size argument val::buffer::overwriteFromStagingBuffer cannot be greater than the size of the val::buffer.\n Size of val::buffer: %d, size of data: %d", _size, dataSize);
		}
		else if (dataSize != _size) {
			printf("VAL: WARNING: the size argument val::buffer::overwriteFromStagingBuffer cannot be greater than the size of the val::buffer.\n Size of val::buffer: %d, size of data: %d", _size, dataSize);
		}
#endif // !NDEBUG

		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* stagingData;
		vkMapMemory(proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(proc._device, stagingBufferMemory);

		proc.copyBuffer(stagingBuffer, _buffer, (VkDeviceSize)dataSize, 0u);

		vkDestroyBuffer(proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(proc._device, stagingBufferMemory, VK_NULL_HANDLE);
	}

	void buffer::overwriteFromBuffer(VAL_PROC& proc, buffer buffer) {
		proc.copyBuffer(_buffer, buffer._buffer, 0u, 0u);
	}

	void buffer::resize(VAL_PROC& proc, uint32_t newSize) {
		// only resize if needed
		if (newSize != _size) {
			VkBuffer tmpBuffer;
			VkDeviceMemory tmpMem;

			// create new buffer and copy the old one into it
			proc.createBuffer(newSize, _usage, bufferSpaceToVkMemoryProperty(_space), tmpBuffer, tmpMem);
			proc.copyBuffer(tmpBuffer, _buffer, 0u, 0u);

			// destroy the old buffer
			vkDestroyBuffer(proc._device, tmpBuffer, VK_NULL_HANDLE);
			vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);

			if (_dataMapped != NULL) {
				void* tmp;// tmp var to prevent memory leak
				if (tmp=realloc(_dataMapped, newSize))
				{
					_dataMapped = tmp;
				}
				else {
					vkUnmapMemory(proc._device, _memory);
					free(_dataMapped);
					_dataMapped = malloc(newSize);
					vkMapMemory(proc._device, _memory, 0u, _size, 0u, &_dataMapped);
				}
			}

			_buffer = tmpBuffer;
			_memory = tmpMem;
			_size = newSize;
		}
	}

	void buffer::destroy(VAL_PROC& proc) {
		free(_dataMapped);
		vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
		vkDestroyBuffer(proc._device, _buffer, VK_NULL_HANDLE);
	}

	const bufferSpace& buffer::getBufferSpace() {
		return _space;
	}

	const VkBuffer& buffer::getVkBuffer() {
		return _buffer;
	}

	const VkDeviceMemory& buffer::getDeviceMemory() {
		return _memory;
	}

	void* buffer::getDataMapped() {
		return _dataMapped;
	}
}