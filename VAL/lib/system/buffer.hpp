#ifndef VAL_BUFFER_HPP
#define VAL_BUFFER_HPP

#include <val/lib/system/system_utils.hpp>

namespace val
{
	/* never host_coherent */
	class Buffer
	{
	public:

		Buffer() = default;

		// creates the buffer from the input values.
		Buffer(ValProc& proc, const uint32_t size, const VkBufferUsageFlags bufferUsage)
		{
			create(proc, size, bufferUsage);
		}

		Buffer& operator=(const Buffer& other) = delete;

	public:

		inline VAL_RETURN_CODE create(ValProc& proc, const uint32_t size, const BUFFER_USAGE bufferUsage) {
			return create(proc, size, (VkBufferUsageFlags)bufferUsage);
		}

		VAL_RETURN_CODE create(ValProc& proc, const uint32_t size, const VkBufferUsageFlags bufferUsage);

		VAL_RETURN_CODE createFromStagingBuffer(Queue& q, const void* data, uint32_t dataSize, const BUFFER_USAGE usages);

		VAL_RETURN_CODE overwriteFromStagingBuffer(Queue& q, const void* data, uint32_t dataSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		void overwriteFromBuffer(Queue& q, Buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

		void resize(Queue& q, uint32_t newSize);

		void destroy(ValProc& proc);

	public:
		const uint32_t& size() const;

		VkBuffer& getVkBuffer();

		const VkDeviceMemory& getDeviceMemory();

		VkBufferUsageFlags getUsageFlags() const;

		VAL_RETURN_CODE setUsages(Queue& q, BUFFER_USAGE usages);

	protected:
		VkBuffer _buffer = NULL;
		VkDeviceMemory _memory = NULL;
		uint32_t _size = 0u;
		VkBufferUsageFlags _usage = 0;
	};
}

#endif VAL_BUFFER_HPP