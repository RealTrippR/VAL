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

	class pipelineCreateInfo {
	public:
		std::vector<shader*> shaders;

	public:
		std::vector<UBO_Handle*> getUniqueUBOs();

		std::vector<pushConstantHandle*> getUniquePushConstants();

		std::vector<SSBO_Handle*> getUniqueSSBOs();

	};
}

#endif // !FML_PIPELINE_CREATE_INFO_HPP