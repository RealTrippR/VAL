#ifndef VAL_BUFFER_HPP
#define VAL_BUFFER_HPP

#include <val/lib/system/system_utils.hpp>

namespace val
{
	class buffer
	{
	public:

		buffer() = default;

		// creates the buffer from the input values.
		buffer(ValProc& proc, const uint32_t& size, const bufferSpace& space, const VkBufferUsageFlags bufferUsage, uint16_t frameCount = 1u)
		{
			create(proc, size, space, bufferUsage, frameCount);
		}
		~buffer() {
			//destroy();
		}

		buffer(ValProc& proc, const buffer& other) {
			this->copyFrom(proc,other);
		}

	public:
		void create(ValProc& proc, const uint32_t& size, const bufferSpace& usage, const VkBufferUsageFlags bufferUsage, uint16_t frameCount = 1u);

		// overwrites from a staging buffer for all frames in flight
		void overwriteFromStagingBuffer(ValProc& proc, void* data, uint64_t dataSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		// overwrites a buffer at dstFrameIdx with from another buffer at srcFrameIdx
		void overwriteFromBuffer(ValProc& proc, buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

		void resize(ValProc& proc, uint32_t newSize);

		void destroy(ValProc& proc);

	public:
		const bufferSpace& getBufferSpace() const;

		const uint32_t& size() const;

		VkBuffer& getVkBuffer();

		const VkDeviceMemory& getDeviceMemory();

		void* getDataMapped();

		VkBufferUsageFlags getUsageFlags() const;

	protected:
		void copyFrom(ValProc& proc, const buffer& src);

	protected:
		VkBuffer _buffer;
		VkDeviceMemory _memory;
		void* _dataMapped;
		bufferSpace _space{};
		uint32_t _size = 0u;
		VkBufferUsageFlags _usage = 0;
	};
}

#endif VAL_BUFFER_HPP