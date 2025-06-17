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


#include <VAL/lib/system/framebuffer.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void Framebuffer::create(VkExtent2D size, VkRenderPass renderpass, VkImageView* attachments, const uint32_t attachmentCount)
	{
		_extent = size;

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass;
		framebufferInfo.attachmentCount = attachmentCount;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _extent.width;
		framebufferInfo.height = _extent.height;
		framebufferInfo.layers = 1;

#ifndef NDEBUG
		if (vkCreateFramebuffer(_proc.getVkLogicalDevice(), &framebufferInfo, nullptr, &_vkframebuffer) != VK_SUCCESS) {
			dbg::printError("Failed to create vkFramebuffer of val::Framebuffer @ %p.", this);
			throw std::runtime_error("Failed to create vkFramebuffer.");
		}
#else
		vkCreateFramebuffer(_proc.getVkLogicalDevice(), &framebufferInfo, nullptr, &renderTargetFramebuffer);
#endif // !NDEBUG
	}

	void Framebuffer::create2(VkExtent2D size, VkRenderPass renderpass, val::ImageView** attachments, const uint32_t attachmentCount)
	{
		tiny_vector<VkImageView> attachmentsVk;
		for (uint32_t i = 0; i < attachmentCount; ++i) {
			attachmentsVk.push_back(*(attachments[i]));
		}
		create(size, renderpass, attachmentsVk.data(), attachmentsVk.size());
	}

	void Framebuffer::create3(VkExtent2D size, VkRenderPass renderpass, tiny_vector<val::ImageView*> attachments)
	{
		create2(size, renderpass, attachments.data(), attachments.size());
	}

	void Framebuffer::destroy() 
	{
		vkDestroyFramebuffer(_proc.getVkLogicalDevice(), _vkframebuffer, NULL);
		_vkframebuffer = NULL;
		_extent = { 0,0 };
	}
}