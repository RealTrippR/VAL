#include <VAL/lib/system/buffer.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void buffer::create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& space, VkBufferUsageFlags bufferUsage) {
		_proc = proc;
		_proc.createBuffer(size, bufferUsage, bufferSpaceToVkMemoryProperty(space), _buffer, _memory);
		_size = size;
		_space = space;
		_usage = bufferUsage;

		// create mapped data memory
		if (CPU_GPU == space) {
			vkMapMemory(_proc._device, _memory, 0u, size, 0u, &_dataMapped);
		}
	}

	void buffer::overwriteFromStagingBuffer(void* data, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
#ifndef NDEBUG
		if (srcOffset > _size) {
			printf("VAL: ERROR: Cannot write out of bounds to a VkBuffer. srcOffset is greater than the size of the buffer.\n Src offset: %llu, buffer size: %llu", srcOffset, _size);
			throw std::logic_error("VAL: ERROR: Cannot write out of bounds to a VkBuffer.srcOffset is greater than the size of the buffer.");
		}
		if (dataSize+dstOffset > _size) {
			printf("VAL: WARNING: the size argument val::buffer::overwriteFromStagingBuffer cannot be greater than the size of the val::buffer.\n Size of val::buffer: %d, size of data: %d", _size, dataSize);
		}
		else if (dataSize != _size) {
			printf("VAL: WARNING: the size argument val::buffer::overwriteFromStagingBuffer cannot be greater than the size of the val::buffer.\n Size of val::buffer: %d, size of data: %d", _size, dataSize);
		}
#endif // !NDEBUG

		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* stagingData;
		vkMapMemory(_proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(_proc._device, stagingBufferMemory);

		_proc.copyBuffer(stagingBuffer, _buffer, (VkDeviceSize)dataSize, srcOffset, dstOffset);

		vkDestroyBuffer(_proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(_proc._device, stagingBufferMemory, VK_NULL_HANDLE);
	}

	void buffer::overwriteFromBuffer(buffer buffer, VkDeviceSize bufferSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
#ifndef NDEBUG
		if (srcOffset > _size) {
			printf("VAL: ERROR: Cannot write out of bounds to a VkBuffer. srcOffset is greater than the size of the buffer.\n Src offset: %llu, buffer size: %llu", srcOffset, _size);
			throw std::logic_error("VAL: ERROR: Cannot write out of bounds to a VkBuffer.srcOffset is greater than the size of the buffer.");
		}
		if (bufferSize + dstOffset > _size) {
			printf("VAL: ERROR: A dst buffer cannot be filled with a src buffer that is greater than the size of the dst buffer.\n Size of dst buffer : %llu, Size of src buffer: %llu\n", _size, bufferSize);
			throw std::logic_error("VAL: ERROR: A dst buffer cannot be filled with a src buffer that is greater than the size of the dst buffer.");
		}
		else if (bufferSize < _size) {
			printf("VAL: Warning: if a buffer is overwitten from a src buffer that has a smaller size than the dst buffer, part of the dst buffer will not be overwritten.");
		}
#endif // !NDEBUG

		_proc.copyBuffer(_buffer, buffer._buffer, bufferSize, srcOffset, dstOffset);
	}

	void buffer::resize(uint32_t newSize) {
		// only resize if needed
		if (newSize != _size) {
			VkBuffer tmpBuffer;
			VkDeviceMemory tmpMem;

			// create new buffer and copy the old one into it
			_proc.createBuffer(newSize, _usage, bufferSpaceToVkMemoryProperty(_space), tmpBuffer, tmpMem);
			_proc.copyBuffer(tmpBuffer, _buffer, 0u, 0u);

			// destroy the old buffer
			vkDestroyBuffer(_proc._device, tmpBuffer, VK_NULL_HANDLE);
			vkFreeMemory(_proc._device, _memory, VK_NULL_HANDLE);

			// remap memory needed
			if (CPU_GPU == _space) {
				vkUnmapMemory(_proc._device, _memory);
				vkMapMemory(_proc._device, tmpMem, 0u, _size, 0u, &_dataMapped);
			}

			_buffer = tmpBuffer;
			_memory = tmpMem;
			_size = newSize;
		}
	}

	void buffer::destroy() {
		if (_dataMapped) {
			vkUnmapMemory(_proc._device, _memory);
			_dataMapped = NULL;
		}
		if (_memory) {
			vkFreeMemory(_proc._device, _memory, VK_NULL_HANDLE);
			_memory = NULL;
		}
		if (_buffer) {
			vkDestroyBuffer(_proc._device, _buffer, VK_NULL_HANDLE);
			_buffer = NULL;
		}
	}

	const bufferSpace& buffer::getBufferSpace() {
		return _space;
	}

	const uint32_t& buffer::size() {
		return _size;
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

	const VkBufferUsageFlags& buffer::getUsageFlags() {
		return _usage;
	}

	VAL_PROC* buffer::getVAL_Proc() {
		return  &_proc;
	}

}