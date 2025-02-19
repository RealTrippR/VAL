#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void UBO_Handle::update(VAL_PROC& proc, void* data) {
#ifndef NDEBUG
		if (_usage != CPU_GPU) {
			printf("VAL: A BUFFER WITH A USAGE BIT OF CPU_GPU CANNOT BE WRITTEN TO OR READ BY THE CPU!\n");
			throw std::runtime_error("VAL: A BUFFER WITH A USAGE BIT OF CPU_GPU CANNOT BE WRITTEN TO OR READ BY THE CPU!");
		}
#endif // !NDEBUG
		memcpy(proc._uniformBuffersMapped[proc._currentFrame][_index],data,_size);
	}

	VkBuffer UBO_Handle::getBuffer(VAL_PROC& proc) {
		return proc._uniformBuffers[proc._currentFrame][_index];
	}

	VkMemoryPropertyFlagBits UBO_Handle::getMemoryPropertyFlagBits() {
		switch (_usage) {
		case GPU_ONLY:
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		case CPU_GPU:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			// this can be optimized, some GPUs support VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			// it would be smart to use this memory property whenever supported.
		}
	}
}