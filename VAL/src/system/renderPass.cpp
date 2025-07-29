#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void RenderPassManager::destroy() {
		if (_VKrenderPass) {
			vkDestroyRenderPass(_procVAL->_device, _VKrenderPass, VK_NULL_HANDLE);
			_VKrenderPass = NULL;
		}
	}

	void RenderPassManager::bindSubpass(Subpass* sp) {
		_subpasses.push_back(sp);
	}

	VkRenderPass& RenderPassManager::getVkRenderPass() {
		return _VKrenderPass;
	}

	void RenderPassManager::update() {
		_VkSubpasses.clear();

		// create std::vector<VkSubpassDescription> _VkSubpasses;
		for (Subpass* sp : _subpasses) {
			sp->update();
			_VkSubpasses.push_back(sp->_subpassDesc);
		}

		// note that _VkAttachments and _attachments have corresponding indices (i.e. _VkAttachments[0] corresponds to _attachments[0] and so on.)
		for (uint_fast32_t i = 0; i < _VkAttachments.size(); ++i)
		{
			VkAttachmentDescription& VKattachment = _VkAttachments[i];
			renderAttachment* VALattachment = _attachments[i];
			if (dynamic_cast<ColorAttachment*>(VALattachment) != NULL
				|| dynamic_cast<DepthAttachment*>(VALattachment) != NULL)
			{
				VKattachment.samples = (VkSampleCountFlagBits)_MSAAsamples;
			}
		}

		createSubpassDependencies();
	}


	void RenderPassManager::setMSAAsamples(VkSampleCountFlags MSAAsamples) {
		_MSAAsamples = (VkSampleCountFlagBits)MSAAsamples;
	}

	VkSampleCountFlagBits RenderPassManager::getMSAAsamples() {
		return _MSAAsamples;
	}

	const std::vector<VkSubpassDependency>& RenderPassManager::createSubpassDependencies() {
		// subpasses must be in a move-forward order (i.e. subpass #2 cannot write to subpass #1)

		_VkSubpassDependencies.resize(_subpasses.size());

		// https://www.reddit.com/r/vulkan/comments/s80reu/subpass_dependencies_what_are_those_and_why_do_i/

		// set source access and stage masks
		for (uint32_t i = 0; i < _subpasses.size(); ++i) 
		{
			val::Subpass& subpass = *_subpasses[i];

			//const tiny_vector<Subpass*>& dependencies = subpass.getDependencies();


			VkSubpassDependency& subDependency = _VkSubpassDependencies[i];

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
				subDependency.dstStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			}

			subDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // This is probably the right one, idk for sure.
		}

		if (_VkSubpassDependencies.size() > 0) { // prevent integer underflow
			// set destination access and stage masks
			for (uint32_t i = 0; i < _VkSubpassDependencies.size() - 1; ++i) {
				VkSubpassDependency& subDependency = _VkSubpassDependencies[i];
				VkSubpassDependency& nextSubDependency = _VkSubpassDependencies[i + 1];

				subDependency.dstStageMask = nextSubDependency.srcStageMask;
				subDependency.dstAccessMask = nextSubDependency.srcAccessMask;

				subDependency.dstSubpass = i;
				nextSubDependency.srcSubpass = i;
				nextSubDependency.dstSubpass = i + 1;
			}
		}

		return _VkSubpassDependencies;
	}

	uint32_t RenderPassManager::addAttachment(val::renderAttachment* attachment) {
		// first check to make sure it hasn't been added yet
		const auto& v = _attachments;
		// Finding the index of val
		auto it = std::find(v.begin(), v.end(), attachment);
		if (it == v.end()) {
			_attachments.push_back(attachment);
			_VkAttachments.push_back(attachment->toVkAttachmentDescription());
			return _attachments.size() - 1;
		}
		return (uint32_t)std::distance(v.begin(), it);
	}
}