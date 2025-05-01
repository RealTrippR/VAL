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


#include <VAL/lib/system/buffer.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void buffer::create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& space, VkBufferUsageFlags bufferUsage, uint16_t frameCount) {
		_proc = proc;
		_size = size;
		_space = space;
		_usage = bufferUsage;
		
		_buffers.resize(frameCount);
		_memory.resize(frameCount);
		_dataMapped.resize(frameCount);

		for (uint16_t fIdx = 0; fIdx < frameCount; ++fIdx) {
			_proc.createBuffer(size, bufferUsage, bufferSpaceToVkMemoryProperty(space), _buffers[fIdx], _memory[fIdx]);
			// create mapped data memory
			if (CPU_GPU == space) {
				vkMapMemory(_proc._device, _memory[fIdx], 0u, size, 0u, &_dataMapped[fIdx]);
			}
		}
	}


	void buffer::overwriteFromStagingBuffer(void* data, uint64_t dataSize, uint16_t frameIdx, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
	{
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, dataSize, srcOffset, dstOffset);
#endif // !NDEBUG

		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* stagingData;
		vkMapMemory(_proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(_proc._device, stagingBufferMemory);

		_proc.copyBuffer(stagingBuffer, _buffers[frameIdx], (VkDeviceSize)dataSize, srcOffset, dstOffset);
		// cleanup staging buffer
		vkDestroyBuffer(_proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(_proc._device, stagingBufferMemory, VK_NULL_HANDLE);
	}

	// overwrites from a staging buffer for all frames in flight
	void buffer::overwriteFromStagingBuffer(void* data, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
		overwriteFromStagingBuffer(data, dataSize, 0u, _buffers.size(), srcOffset, dstOffset);
	}

	// overwrites from a staging buffer for all frames within the specified range 
	void buffer::overwriteFromStagingBuffer(void* data, uint64_t dataSize, uint16_t frameIdxBegin, uint16_t frameRangeEnd, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
	{
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, dataSize, srcOffset, dstOffset);
#endif // !NDEBUG

		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* stagingData;
		vkMapMemory(_proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(_proc._device, stagingBufferMemory);

		for (uint8_t fIdx = frameIdxBegin; fIdx < frameRangeEnd; ++fIdx) {
			_proc.copyBuffer(stagingBuffer, _buffers[fIdx], (VkDeviceSize)dataSize, srcOffset, dstOffset);
		}

		// cleanup staging buffer
		vkDestroyBuffer(_proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(_proc._device, stagingBufferMemory, VK_NULL_HANDLE);
	}



	void buffer::overwriteFromBuffer(buffer& srcBuffer, VkDeviceSize srcBufferRange, uint16_t srcFrameIdx, uint16_t dstFrameIdx, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, srcBufferRange, srcOffset, dstOffset);
#endif // !NDEBUG

		_proc.copyBuffer(_buffers[dstFrameIdx], srcBuffer._buffers[srcFrameIdx], srcBufferRange, srcOffset, dstOffset);
	}

	// overwrites all buffers for every frame in flight;
	// both buffers must have the same number of frames in flight
	void buffer::overwriteFromBuffer(buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
#ifndef NDEBUG
		if (getFrameCount() != srcBuffer.getFrameCount()) {
			printf("VAL::ERROR: Cannot overwrite all frames of the this buffer with the source buffer, as the number of frames in flight are not equal!\n\t%d ≠ %d frames\n", getFrameCount(),srcBuffer.getFrameCount());
			throw std::runtime_error("VAL::ERROR: Cannot overwrite all frames of the this buffer with the source buffer, as the number of frames in flight are not equal!");
		}
#endif // !NDEBUG

		for (uint32_t fIdx = 0; fIdx < _buffers.size(); ++fIdx) {
			overwriteFromBuffer(srcBuffer, srcBufferRange, fIdx, fIdx, srcOffset, dstOffset);
		}
	}

	void buffer::resize(uint32_t newSize) {
		// only resize if needed
		if (newSize != _size) {
			for (uint8_t fIdx = 0; fIdx < _buffers.size(); ++fIdx) {
				VkBuffer tmpBuffer;
				VkDeviceMemory tmpMem;

				// create new buffer and copy the old one into it
				_proc.createBuffer(newSize, _usage, bufferSpaceToVkMemoryProperty(_space), tmpBuffer, tmpMem);
				_proc.copyBuffer(tmpBuffer, _buffers[fIdx], 0u, 0u);

				// destroy the old buffer
				vkDestroyBuffer(_proc._device, tmpBuffer, VK_NULL_HANDLE);
				vkFreeMemory(_proc._device, _memory[fIdx], VK_NULL_HANDLE);

				// remap memory needed
				if (CPU_GPU == _space) {
					vkUnmapMemory(_proc._device, _memory[fIdx]);
					vkMapMemory(_proc._device, tmpMem, 0u, _size, 0u, &_dataMapped[fIdx]);
				}

				_buffers[fIdx] = tmpBuffer;
				_memory[fIdx] = tmpMem;
				_size = newSize;
			}
		}
	}

	void buffer::destroy() {
		_dataMapped.clear();
		for (uint8_t fIdx = 0; fIdx < _buffers.size(); ++fIdx) {
			
			if (_memory[fIdx]) {
				vkFreeMemory(_proc._device, _memory[fIdx], VK_NULL_HANDLE);
			}
			if (_buffers[fIdx]) {
				vkDestroyBuffer(_proc._device, _buffers[fIdx], VK_NULL_HANDLE);
			}
		}
		_buffers.clear();
		_memory.clear();
	}

	const bufferSpace& buffer::getBufferSpace() {
		return _space;
	}

	const uint32_t& buffer::getFrameCount() {
		return _buffers.size();
	}

	const uint32_t& buffer::size() {
		return _size;
	}

	const VkBuffer& buffer::getVkBuffer(const uint8_t frameIdx) {
		return _buffers[frameIdx];
	}

	const VkDeviceMemory& buffer::getDeviceMemory(const uint8_t frameIdx) {
		return _memory[frameIdx];
	}

	void* buffer::getDataMapped(const uint8_t frameIdx) {
		return _dataMapped[frameIdx];
	}

	const VkBufferUsageFlags& buffer::getUsageFlags() {
		return _usage;
	}

	VAL_PROC* buffer::getVAL_Proc() {
		return  &_proc;
	}

	////////////////////////////////////////////////////////////////////////////
	void buffer::copy(const buffer& other) {
		const uint32_t frameCount = other._buffers.size();
		// cleanup old data.
		this->destroy();

		_size = other._size;
		_space = other._space;
		_proc = other._proc;

		_buffers.resize(frameCount);
		_memory.resize(frameCount);
		_dataMapped.resize(frameCount);

		
		for (uint16_t fIdx = 0; fIdx < frameCount; ++fIdx) {
			_proc.createBuffer(other._size, other._usage, bufferSpaceToVkMemoryProperty(_space), _buffers[fIdx], _memory[fIdx]);
			// create mapped data memory
			if (CPU_GPU == _space) {
				vkMapMemory(_proc._device, _memory[fIdx], 0u, _size, 0u, &_dataMapped[fIdx]);
			}
			_proc.copyBuffer(other._buffers[fIdx], _buffers[fIdx], _size);
		}
	}
}