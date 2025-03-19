#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void renderPassManager::destroy() {
		if (_VKrenderPass) {
			vkDestroyRenderPass(_procVAL->_device, _VKrenderPass, VK_NULL_HANDLE);
			_VKrenderPass = NULL;
		}
	}

	void renderPassManager::bindSubpass(subpass* sp) {
		_subpasses.push_back(sp);
	}

	VkRenderPass& renderPassManager::getVkRenderPass() {
		return _VKrenderPass;
	}

	void renderPassManager::update() {
		_VkSubpasses.clear();
		uint32_t attachIdx = 0u;
		// create std::vector<VkSubpassDescription> _VkSubpasses;
		for (subpass* sp : _subpasses) {
			sp->update();
			_VkSubpasses.push_back(sp->_subpassDesc);
		}

		createSubpassDependencies();
	}

	const std::vector<VkSubpassDependency>& renderPassManager::createSubpassDependencies() {
		// subpasses must be in a move-forward order (i.e. subpass #2 cannot write to subpass #1)

		_VkSubpassDependencies.resize(_subpasses.size());

		// https://www.reddit.com/r/vulkan/comments/s80reu/subpass_dependencies_what_are_those_and_why_do_i/

		// set source access and stage masks
		for (uint32_t i = 0; i < _subpasses.size(); ++i) {

			VkSubpassDependency& subDependency = _VkSubpassDependencies[i];
			val::subpass& subpass = *_subpasses[i];

			if (i < _subpasses.size() - 1) {
				VkSubpassDependency* nextDepenency = &(_VkSubpassDependencies[i + 1]);
			}
			if (i == 0) {
				subDependency.srcSubpass = VK_SUBPASS_EXTERNAL; // Synchronizes an external process (previous frame) with the first subpass.
			}
			else {
				subDependency.srcSubpass = i - 1; // Previous subpass
			}

			if (subpass._colorAttachments.size() > 0) {
				subDependency.dstStageMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				subDependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}

			if (subpass._depthStencilAttachment.has_value()) {
				subDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				subDependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			subDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // This is probably the right one, idk for sure.
		}

		// set destination access and stage masks
		for (uint32_t i = 0; i < _VkSubpassDependencies.size()-1; ++i) {
			VkSubpassDependency& subDependency = _VkSubpassDependencies[i];
			VkSubpassDependency& nextSubDependency = _VkSubpassDependencies[i + 1];

			subDependency.dstStageMask = nextSubDependency.srcStageMask;
			subDependency.dstAccessMask = nextSubDependency.srcAccessMask;

			subDependency.dstSubpass = i;
			nextSubDependency.srcSubpass = i;
			nextSubDependency.dstSubpass = i + 1;
		}

		return _VkSubpassDependencies;
	}

	uint32_t renderPassManager::addAttachment(val::renderAttachment* attachment) {
		// first check to make sure it hasn't been added yet
		const auto& v = _attachments;
		// Finding the index of val
		auto it = std::find(v.begin(), v.end(), attachment);
		if (it == v.end()) {
			_attachments.push_back(attachment);
			_VkAttachments.push_back(attachment->toVkAttachmentDescription());
			return _attachments.size() - 1;
		}
		return std::distance(v.begin(), it);
	}
}