#ifndef VAL_PIPELINE_TYPE_ENUM_HPP
#define VAL_PIPELINE_TYPE_ENUM_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	enum PIPELINE_TYPE {
		GRAPHICS,
		COMPUTE
	};

	constexpr VkPipelineBindPoint PIPELINE_TYPE_To_VkPipelineBindPoint(const PIPELINE_TYPE& type) {
		VkPipelineBindPoint point;
		switch (type) {
		case GRAPHICS:
			return VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		case COMPUTE:
			return VK_PIPELINE_BIND_POINT_COMPUTE;
			break;
		default:
			printf("VAL: WARNING: Invalid pipeline type input into PIPELINE_TYPE_To_VkPipelineBindPoint!\n");
			return VK_PIPELINE_BIND_POINT_MAX_ENUM; // failure
		}
	}
}

#endif // !VAL_PIPELINE_TYPE_ENUM_HPP