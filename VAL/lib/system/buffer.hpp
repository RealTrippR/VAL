#ifndef VAL_BUFFER_HPP
#define VAL_BUFFER_HPP

#include <val/lib/system/system_utils.hpp>

namespace val
{
	class buffer
	{
	public:

		buffer(VAL_PROC& proc) : _proc(proc) {}

		buffer(VAL_PROC& proc, const uint32_t& size, const bufferSpace& space, const VkBufferUsageFlags bufferUsage)
			: _proc(proc) {
			create(proc, size, space, bufferUsage);
		}
		~buffer() {
			destroy();
		}

		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

	public:
		void create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& usage, const VkBufferUsageFlags bufferUsage);

		void overwriteFromStagingBuffer(void* data, uint64_t dataSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		void overwriteFromBuffer(buffer buffer, VkDeviceSize bufferSize, VkDeviceSize srcOffset = 0U, VkDeviceSize dstOffset = 0U);

		void resize(uint32_t newSize);

		void destroy();

	public:
		const bufferSpace& getBufferSpace();

		const uint32_t& size();

		const VkBuffer& getVkBuffer();

		const VkDeviceMemory& getDeviceMemory();

		void* getDataMapped();

		const VkBufferUsageFlags& getUsageFlags();

		VAL_PROC* getVAL_Proc();

	protected:
		VAL_PROC& _proc;  // Store a reference
		uint32_t _size = 0u;
		bufferSpace _space{};
		VkBufferUsageFlags _usage = 0;
		VkBuffer _buffer = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		void* _dataMapped = NULL;
	};
}

#endif VAL_BUFFER_HPP