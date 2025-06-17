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

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/imageView.hpp>

namespace val {
	class Framebuffer
	{
	public:
		Framebuffer(ValProc& proc, const uint32_t width, const uint32_t height, VkRenderPass renderpass, VkImageView attachment)
			: _proc(proc)
		{
			create({ width , height }, renderpass, &attachment, 1);
		}
		Framebuffer(ValProc& proc, VkExtent2D size, VkRenderPass renderpass, VkImageView attachment)
			: _proc(proc)
		{
			create(size, renderpass, &attachment, 1);
		}
		Framebuffer(ValProc& proc, const uint32_t width, const uint32_t height, VkRenderPass renderpass, ImageView& attachment)
			: _proc(proc)
		{
			create({ width , height }, renderpass, &(attachment.getImageView()), 1);
		}
		Framebuffer(ValProc& proc, VkExtent2D size, VkRenderPass renderpass, ImageView& attachment)
			: _proc(proc)
		{
			create(size, renderpass, &(attachment.getImageView()), 1);
		}

		Framebuffer(ValProc& proc, const uint32_t width, const uint32_t height, VkRenderPass renderpass, tiny_vector<VkImageView> attachments) 
			: _proc(proc)
		{
			create({ width , height }, renderpass, attachments.data(), attachments.size());
		}
		Framebuffer(ValProc& proc, VkExtent2D size, VkRenderPass renderpass, tiny_vector<VkImageView> attachments)
			: _proc(proc)
		{
			create(size, renderpass, attachments.data(), attachments.size());
		}

		Framebuffer(ValProc& proc, const uint32_t width, const uint32_t height, VkRenderPass renderpass, tiny_vector<val::ImageView*> attachments)
			: _proc(proc)
		{
			create3({width, height}, renderpass, attachments);
		}

		Framebuffer(ValProc& proc, VkExtent2D size, VkRenderPass renderpass, tiny_vector<val::ImageView*> attachments)
			: _proc(proc)
		{
			create3(size, renderpass, attachments);
		}

		~Framebuffer() {
			destroy();
		}

		// conversion operator to VkFramebuffer
		operator VkFramebuffer () const {
			return _vkframebuffer;
		}

		// conversion operator to VkFramebuffer&
		operator const VkFramebuffer&() const {
			return _vkframebuffer;
		}

		// conversion operator to const VkFramebuffer&
		operator VkFramebuffer& () {
			return _vkframebuffer;
		}
	public:
		inline VkExtent2D getSize() const;

		inline VkFramebuffer getVkFramebuffer() const;

	private:
		void create(VkExtent2D size, VkRenderPass renderpass, VkImageView* attachments, const uint32_t attachmentCount);

		void create2(VkExtent2D size, VkRenderPass renderpass,  val::ImageView** attachments, const uint32_t attachmentCount);

		void create3(VkExtent2D size, VkRenderPass renderpass, tiny_vector<val::ImageView*> attachments);

		void destroy();

	private:
		ValProc& _proc;
		VkExtent2D _extent = { 0,0 };
		VkFramebuffer _vkframebuffer = VK_NULL_HANDLE;
	};
}