#ifndef VAL_RENDER_PASS_HPP
#define VAL_RENDER_PASS_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/subpass.hpp>
#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val {
	class renderPassManager
	{
	public:
		void bindSubpass(subpass* sp);

		VkRenderPass getVkRenderPass();

		const std::vector<VkSubpassDependency>& createSubpassDependencies();

		void update() {
			_VkAttachments.clear();
			// create std::vector<VkSubpassDescription> _VkSubpasses;
			for (subpass* sp : _subpasses) {
				sp->update();
				for (renderAttachment* attachment : sp->_attachments) {
					_VkAttachments.push_back(attachment->toVkAttachmentDescription());
				}
				_VkSubpasses.push_back(sp->_subpassDesc);
			}

			createSubpassDependencies();
		}

	protected:
		friend subpass;
		friend VAL_PROC;
		// these must be in a move-forward order (i.e. subpass #2 cannot write to subpass #1)
		std::vector<subpass*> _subpasses;

		std::vector<VkSubpassDescription> _VkSubpasses;

		std::vector<VkAttachmentDescription> _VkAttachments;

		std::vector<VkSubpassDependency> _VkSubpassDependencies;

		VkRenderPass _renderPass = VK_NULL_HANDLE;
	};
}

#endif // !VAL_RENDER_PASS_HPP