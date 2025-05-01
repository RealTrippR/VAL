#ifndef FML_COMPUTE_PIPELINE_CREATE_INFO_HPP
#define FML_COMPUTE_PIPELINE_CREATE_INFO_HPP

#define GLFW_INCLUDE_VULKAN

#include <VAL/lib/system/pipelineCreateInfo.hpp>
namespace val {
	class shader;

	class computePipelineCreateInfo : public pipelineCreateInfo {
	public:
	public:
		computePipelineCreateInfo() { _bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE; }
	public:
		inline VkPipelineBindPoint getBindPoint() { return _bindPoint; };
	};
}

#endif // !FML_COMPUTE_PIPELINE_CREATE_INFO_HPP