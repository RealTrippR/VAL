#ifndef VAL_RENDER_PASS_INFO_HPP
#define VAL_RENDER_PASS_INFO_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>

#include <vector>
#include <utility>      // std::pair

namespace val {
	struct renderPassInfo
	{
		std::vector<VkAttachmentDescription> attachments;

		//std::vector<VkImageLayout> attachmentImageLayouts;

		std::vector<VkSubpassDescription> subpasses;

		std::vector<VkSubpassDependency> subpassDependencies;
	};
}

#endif // !VAL_RENDER_PASS_INFO_HPP