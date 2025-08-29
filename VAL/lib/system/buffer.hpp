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

		inline void create(ValProc& proc, const uint32_t size, const BUFFER_USAGE bufferUsage) {
			create(proc, size, (VkBufferUsageFlags)bufferUsage);
		}

		void create(ValProc& proc, const uint32_t size, const VkBufferUsageFlags bufferUsage);

		void createFromStagingBuffer(ValProc& proc, const void* data, uint32_t dataSize, const BUFFER_USAGE usages);

		void overwriteFromStagingBuffer(ValProc& proc, const void* data, uint32_t dataSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		void overwriteFromBuffer(ValProc& proc, Buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

		void resize(ValProc& proc, uint32_t newSize);

		void destroy(ValProc& proc);

	public:
		const uint32_t& size() const;

		VkBuffer& getVkBuffer();

		const VkDeviceMemory& getDeviceMemory();

		VkBufferUsageFlags getUsageFlags() const;

		VAL_RETURN_CODE setUsages(ValProc& proc, BUFFER_USAGE usages);

	protected:
		VkBuffer _buffer = NULL;
		VkDeviceMemory _memory = NULL;
		uint32_t _size = 0u;
		VkBufferUsageFlags _usage = 0;
	};
}

#endif VAL_BUFFER_HPP