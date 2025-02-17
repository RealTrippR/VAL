#ifndef VAL_UBO_HANDLE_HPP
#define VAL_UBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	struct UBO_Handle {
		UBO_Handle() = default;
		UBO_Handle(uint16_t sizeOfUBO) {
			_size = sizeOfUBO;
		}
	public:
		void update(VAL_PROC& proc, void* data);
	public:
		uint16_t _size = 0; // Vulkan allows for 16000 as the max size in bytes of a UBO buffer
		int _index = 0;

		VkShaderStageFlagBits stageFlags;
	};
}

#endif // !VAL_UBO_HANDLE_HPP