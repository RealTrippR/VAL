#ifndef VAL_BUFFER_HPP
#define VAL_BUFFER_HPP

#include <val/lib/system/system_utils.hpp>

namespace val
{
	class buffer
	{
	public:
		buffer() = default;
		buffer(VAL_PROC& proc, const uint32_t& size, const bufferSpace& usage, const VkBufferUsageFlags bufferUsage) {
			create(proc, size, usage, bufferUsage);
		}
		buffer(const buffer& other) {
			create(proc, size, usage, bufferUsage);
		}

		buffer operator=(buffer const& other)
		{
			create(proc, size, usage, bufferUsage);
		}
	public:
		void create(VAL_PROC& proc, const uint32_t& size, const bufferSpace& usage, const VkBufferUsageFlags bufferUsage);

		void overwriteFromStagingBuffer(VAL_PROC& proc, void* data, uint32_t size);

		void overwriteFromBuffer(VAL_PROC& proc, buffer buffer);

		void resize(VAL_PROC& proc, uint32_t newSize);

		//void update(void* data);
		void destroy(VAL_PROC& proc);

	public:
		const bufferSpace& getBufferSpace();

		const VkBuffer& getVkBuffer();

		const VkDeviceMemory& getDeviceMemory();

		void* getDataMapped();

	protected:
		uint32_t _size;
		bufferSpace _space;
		VkBufferUsageFlags _usage;
		VkBuffer _buffer = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		void* _dataMapped = NULL;
	};
}

#endif VAL_BUFFER_HPP