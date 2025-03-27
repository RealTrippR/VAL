#ifndef VAL_RENDER_PASS_HPP
#define VAL_RENDER_PASS_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/subpass.hpp>
#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val {
	class renderPassManager
	{
	public:
		renderPassManager(VAL_PROC& proc) { _procVAL = &proc; };
		renderPassManager(VAL_PROC& proc, VkSampleCountFlagBits MSAAsamples) { _procVAL = &proc; _MSAAsamples = MSAAsamples; };

		~renderPassManager() {
			destroy();
		}
	public:

		void destroy();

		void bindSubpass(subpass* sp);

		VkRenderPass& getVkRenderPass();

		const std::vector<VkSubpassDependency>& createSubpassDependencies();

		void update();

		void setMSAAsamples(VkSampleCountFlags MSAAsamples);

		VkSampleCountFlagBits getMSAAsamples();

	protected:
		// returns the index of the attachment
		// if the attachment is already in the list of attachments, it won't be added,
		// but it will still return it's index
		uint32_t addAttachment(val::renderAttachment* attachment);

	protected:
		friend renderAttachment;
		friend subpass;
		friend VAL_PROC;
		/******************************************/
		VAL_PROC* _procVAL = NULL;
		/******************************************/

		VkSampleCountFlagBits _MSAAsamples = VK_SAMPLE_COUNT_1_BIT;

		// these must be in a move-forward order (i.e. subpass #2 cannot write to subpass #1)
		std::vector<subpass*> _subpasses;

		std::vector<VkSubpassDescription> _VkSubpasses;

		std::vector<val::renderAttachment*> _attachments;

		std::vector<VkAttachmentDescription> _VkAttachments;

		std::vector<VkSubpassDependency> _VkSubpassDependencies;

		VkRenderPass _VKrenderPass = VK_NULL_HANDLE;
	};
}

#endif // !VAL_RENDER_PASS_HPP