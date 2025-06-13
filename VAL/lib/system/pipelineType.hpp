#ifndef VAL_PIPELINE_TYPE_ENUM_HPP
#define VAL_PIPELINE_TYPE_ENUM_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	enum PIPELINE_TYPE  : uint8_t{
		Graphics = VK_PIPELINE_BIND_POINT_GRAPHICS,
		Compute = VK_PIPELINE_BIND_POINT_COMPUTE
	};

	constexpr VkPipelineBindPoint PIPELINE_TYPE_To_VkPipelineBindPoint(const PIPELINE_TYPE& type) {
#ifndef NDEBUG
		if (type != PIPELINE_TYPE::Graphics && type != PIPELINE_TYPE::Compute) {
			dbg::printError("PIPELINE_TYPE_To_VkPipelineBindPoint was called on an invalid PIPELINE_TYPE enum. As an integer, it's value is: %d.", (int)type);
		}
#endif // !NDEBUG

		return (VkPipelineBindPoint)type;
	}
}

#endif // !VAL_PIPELINE_TYPE_ENUM_HPP