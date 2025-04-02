#ifndef VAL_BUFFER_HPP
#define VAL_BUFFER_HPP

#include <val/lib/system/system_utils.hpp>

namespace val
{
	class buffer
	{
	public:

		buffer(VAL_PROC& proc) : _proc(proc) {}

		// creates the buffer from the input values.
		buffer(VAL_PROC& proc, const uint32_t& size, const bufferSpace& space, const VkBufferUsageFlags bufferUsage, uint16_t frameCount = 1u)
			: _proc(proc) {
			create(proc, size, space, bufferUsage, frameCount);
		}
		~buffer() {
			destroy();
		}

		buffer(const buffer& other) : _proc(other._proc) {
			this->copy(other);
		}
		buffer& operator=(const buffer& other) {
			this->copy(other);
		}

	public:
		void create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& usage, const VkBufferUsageFlags bufferUsage, uint16_t frameCount = 1u);

		void overwriteFromStagingBuffer(void* data, uint64_t dataSize, uint16_t frameIdx, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		// overwrites from a staging buffer for all frames in flight
		void overwriteFromStagingBuffer(void* data, uint64_t dataSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		// overwrites from a staging buffer for all frames within the specified range 
		void overwriteFromStagingBuffer(void* data, uint64_t dataSize, uint16_t frameIdxBegin, uint16_t frameRangeEnd, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		// overwrites a buffer at dstFrameIdx with from another buffer at srcFrameIdx
		void overwriteFromBuffer(buffer& srcBuffer, VkDeviceSize srcBufferRange, uint16_t srcFrameIdx, uint16_t dstFrameIdx, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);
		
		// overwrites all buffers for every frame in flight;
		// both buffers must have the same number of frames in flight
		void overwriteFromBuffer(buffer& srcBuffer, VkDeviceSize srcBufferRange, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		void resize(uint32_t newSize);

		void destroy();

	public:
		const bufferSpace& getBufferSpace();

		const uint32_t& getFrameCount();

		const uint32_t& size();

		const VkBuffer& getVkBuffer(const uint8_t frameIdx = 0);

		const VkDeviceMemory& getDeviceMemory(const uint8_t frameIdx);

		void* getDataMapped(const uint8_t frameIdx = 0u);

		const VkBufferUsageFlags& getUsageFlags();

		VAL_PROC* getVAL_Proc();

	protected:
		void copy(const buffer& other);

	protected:
		VAL_PROC& _proc;  // Store a reference
		uint32_t _size = 0u;
		bufferSpace _space{};
		VkBufferUsageFlags _usage = 0;
		// each vector has a size equivalent to the frame count that it was initialized with
		std::vector<VkBuffer> _buffers; // it would be more efficient to pack this data into just 1 buffer. i.e. VkBuffer* (which includes the data for both frames that can be accessed with an offset)
		std::vector<VkDeviceMemory> _memory;
		std::vector<void*> _dataMapped;
	};
}

#endif VAL_BUFFER_HPP