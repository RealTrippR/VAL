#ifndef FML_PIPELINE_CREATE_INFO_HPP
#define FML_PIPELINE_CREATE_INFO_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/renderPassInfo.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>
#include <vector>
#include <algorithm>

namespace val {
	class shader;

	class computePipelineCreateInfo {
	public:
		std::vector<shader*> shaders;

	public:
		virtual std::vector<UBO_Handle*> getUniqueUBOs();

		virtual std::vector<pushConstantHandle*> getUniquePushConstants();

		virtual std::vector<SSBO_Handle*> getUniqueSSBOs();

	};
}

#endif // !FML_COMPUTE_PIPELINE_CREATE_INFO_HPP