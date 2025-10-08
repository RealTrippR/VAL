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
	VAL_RETURN_CODE Buffer::create(ValProc& proc, const uint32_t size, VkBufferUsageFlags bufferUsage) {
		_size = size;
		_usage = bufferUsage;
		
		proc.createBuffer(size, bufferUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory);
		if (!_buffer) {
			return VAL_FAILURE;
		}
		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE Buffer::createFromStagingBuffer(Queue& q, const void* data, uint32_t dataSize, const BUFFER_USAGE usages)
	{

		auto& proc = *q.getValProc();
		if (create(proc, dataSize, usages) != VAL_SUCCESS) {
			return VAL_FAILURE;
		}
		overwriteFromStagingBuffer(q, data, dataSize, 0, 0);
		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE Buffer::overwriteFromStagingBuffer(Queue& q, const void* data, uint32_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
	{
		if ((uint64_t)dataSize + dstOffset > UINT32_MAX) {
			return VAL_FAILURE;
		}

		auto& proc = *q.getValProc();

		if (_size < dataSize + dstOffset) {
			resize(q, dataSize + dstOffset);
		}
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, dataSize, srcOffset, dstOffset);
#endif // !NDEBUG

		// create staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		if (proc.createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory) != VAL_SUCCESS) {
			return VAL_FAILURE;
		}

		void* stagingData;
		vkMapMemory(proc._device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
		memcpy(stagingData, data, (size_t)dataSize);
		vkUnmapMemory(proc._device, stagingBufferMemory);

		proc.copyBuffer(q, q.getCmdPool(), stagingBuffer, _buffer, (VkDeviceSize)dataSize, srcOffset, dstOffset);

		// cleanup staging buffer
		vkDestroyBuffer(proc._device, stagingBuffer, VK_NULL_HANDLE);
		vkFreeMemory(proc._device, stagingBufferMemory, VK_NULL_HANDLE);

		return VAL_SUCCESS;
	}



	void Buffer::overwriteFromBuffer(Queue& q, Buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
	{
		auto& proc = *q.getValProc();
		if (_size < srcBufferRange) {
			resize(q, (uint32_t)srcBufferRange);
		}
#ifndef NDEBUG
		__VAL_DEBUG_ValidateBufferCopy(_size, srcBufferRange, srcOffset, dstOffset);
#endif // !NDEBUG

		proc.copyBuffer(q, q.getCmdPool(), _buffer, srcBuffer._buffer, srcBufferRange, srcOffset, dstOffset);
	}

	void Buffer::resize(Queue& q, uint32_t newSize) {
		auto& proc = *q.getValProc();
		if (_buffer == NULL) {
			create(proc, newSize, _usage);
			return;
		}
		// only resize if needed
		if (newSize != _size) {
			VkBuffer tmpBuffer;
			VkDeviceMemory tmpMem;

			// create new buffer and copy the old one into it
			proc.createBuffer(newSize, _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tmpBuffer, tmpMem);
			proc.copyBuffer(q, q.getCmdPool(), _buffer, tmpBuffer, _size);
			// destroy the old buffer
			vkDestroyBuffer(proc._device, _buffer, VK_NULL_HANDLE);
			vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
			

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
		_size = 0u;
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

	VkBufferUsageFlags Buffer::getUsageFlags() const
	{
		return _usage;
	}

	VAL_RETURN_CODE Buffer::setUsages(Queue& q, BUFFER_USAGE usages)
	{
		auto& proc = *q.getValProc();
		_usage = usages;
		if (!_buffer || _size == 0u) {
			return VAL_SUCCESS;
		}
		else {
			VkDeviceMemory stagingMem;
			VkBuffer stagingBuffer=NULL;
			
			proc.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | (VkBufferUsageFlags)usages, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, stagingBuffer, stagingMem);
			if (!stagingBuffer)
				return VAL_FAILURE;
			proc.copyBuffer(q,q.getCmdPool(),_buffer, stagingBuffer, _size);

			vkFreeMemory(proc, _memory, NULL);
			vkDestroyBuffer(proc, _buffer, NULL);

			_memory = stagingMem;
			_buffer = stagingBuffer;
		}
		return VAL_SUCCESS;
	}
}