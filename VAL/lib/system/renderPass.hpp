/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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