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

	VkBuffer UBO_Handle::getCurrentBuffer(VAL_PROC& proc) {
		return proc._uniformBuffers[proc._currentFrame][_index];
	}

	std::vector<VkBuffer> UBO_Handle::getBuffers(VAL_PROC& proc) {
		std::vector<VkBuffer> buffers;
		for (uint_fast8_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			buffers.push_back(proc._uniformBuffers[i][_index]);
		}
		return buffers;
	}


	VkMemoryPropertyFlags UBO_Handle::getMemoryPropertyFlags() {
		if (GPU_ONLY) {
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		else if (CPU_GPU)
		{
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}
	}
}