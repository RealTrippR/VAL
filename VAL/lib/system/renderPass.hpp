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
	class RenderPassManager
	{
	public:
		RenderPassManager(ValProc& proc) { _procVAL = &proc; };
		RenderPassManager(ValProc& proc, VkSampleCountFlagBits MSAAsamples) { _procVAL = &proc; _MSAAsamples = MSAAsamples; };

		~RenderPassManager() {
			destroy();
		}
	public:

		void destroy();

		void bindSubpass(Subpass* sp);

		VkRenderPass& getVkRenderPass();

		const tiny_vector<VkSubpassDependency>& createSubpassDependencies();

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
		friend Subpass;
		friend ValProc;
		/******************************************/
		ValProc* _procVAL = NULL;
		/******************************************/

		// these must be in a move-forward order (i.e. subpass #2 cannot write to subpass #1)
		tiny_vector<Subpass*> _subpasses;

		tiny_vector<VkSubpassDescription> _VkSubpasses;

		tiny_vector<val::renderAttachment*> _attachments;

		tiny_vector<VkAttachmentDescription> _VkAttachments;

		tiny_vector<VkSubpassDependency> _VkSubpassDependencies;

		VkRenderPass _VKrenderPass = VK_NULL_HANDLE;

		VkSampleCountFlagBits _MSAAsamples = VK_SAMPLE_COUNT_1_BIT;
	};
}

#endif // !VAL_RENDER_PASS_HPP