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
	void Buffer::create(ValProc& proc, const uint32_t& size, const bufferSpace& space, VkBufferUsageFlags bufferUsage, uint16_t frameCount) {
		_size = size;
		_space = space;
		_usage = bufferUsage;
		
		proc.createBuffer(size, bufferUsage, bufferSpaceToVkMemoryProperty(space), _buffer, _memory);
		// create mapped data memory
		if (CPU_GPU == space) {
			vkMapMemory(proc._device, _memory, 0u, size, 0u, &_dataMapped);
		}
	}

	// overwrites from a staging buffer for all frames in flight
	void Buffer::overwriteFromStagingBuffer(ValProc& proc, void* data, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset) 
	{
		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* stagingData;
		vkMapMemory(proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(proc._device, stagingBufferMemory);

		proc.copyBuffer(stagingBuffer, _buffer, (VkDeviceSize)dataSize, srcOffset, dstOffset);

		// cleanup staging buffer
		vkDestroyBuffer(proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(proc._device, stagingBufferMemory, VK_NULL_HANDLE);
	}



	void Buffer::overwriteFromBuffer(ValProc& proc, Buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, srcBufferRange, srcOffset, dstOffset);
#endif // !NDEBUG

		proc.copyBuffer(_buffer, srcBuffer._buffer, srcBufferRange, srcOffset, dstOffset);
	}

	void Buffer::resize(ValProc& proc, uint32_t newSize) {
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

			// remap memory needed
			if (CPU_GPU == _space) {
				vkUnmapMemory(proc._device, _memory);
				vkMapMemory(proc._device, tmpMem, 0u, _size, 0u, &_dataMapped);
			}

			_buffer = tmpBuffer;
			_memory  = tmpMem;
			_size = newSize;
		}
	}

	void Buffer::destroy(ValProc& proc) {
		if (_memory) {
			vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
			_memory = VK_NULL_HANDLE;
		}
		if (_buffer) {
			vkDestroyBuffer(proc.getVkLogicalDevice(), _buffer, VK_NULL_HANDLE);
			_buffer = VK_NULL_HANDLE;
		}
	}

	const bufferSpace& Buffer::getBufferSpace() const  {
		return _space;
	}

	const uint32_t& Buffer::size() const {
		return _size;
	}

	VkBuffer& Buffer::getVkBuffer() {
		return _buffer;
	}

	const VkDeviceMemory& Buffer::getDeviceMemory() {
		return _memory;
	}

	void* Buffer::getDataMapped() {
		return _dataMapped;
	}


	VkBufferUsageFlags Buffer::getUsageFlags() const
	{
		return _usage;
	}
	////////////////////////////////////////////////////////////////////////////
	void Buffer::copyFrom(ValProc& proc, const Buffer& other) 
	{
		// cleanup old data.
		this->destroy(proc);

		_size = other._size;
		_space = other._space;


		
		proc.createBuffer(other._size, other._usage, bufferSpaceToVkMemoryProperty(_space), _buffer, _memory);
		// create mapped data memory
		if (CPU_GPU == _space) {
			vkMapMemory(proc._device, _memory, 0u, _size, 0u, &_dataMapped);
		}
		proc.copyBuffer(other._buffer, _buffer, _size);
	}
}