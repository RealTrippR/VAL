#ifndef VAL_SSBO_HANDLE_HPP
#define VAL_SSBO_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>
namespace val {
	class SSBO_Handle {
		SSBO_Handle() = default;
		SSBO_Handle(uint16_t sizeOfSSBO) {
			_size = sizeOfSSBO;
		}
	public:
		void update(VAL_PROC& proc, void* data);

		void resize(VAL_PROC& proc, size_t size);
	public:
		uint64_t _size = 0;
		int _index = 0;

		VkShaderStageFlags _stageFlags = 0;
	};
}

#endif // !VAL_SSBO_HANDLE_HPP