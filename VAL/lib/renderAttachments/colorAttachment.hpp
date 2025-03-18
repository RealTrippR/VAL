#ifndef VAL_COLOR_ATTACHMENT_HPP
#define VAL_COLOR_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class colorAttachment : public renderAttachment
	{
	public:
		colorAttachment() : renderAttachment() {}
		colorAttachment(RENDER_ATTACHMENT_USAGE usage) : renderAttachment(usage) {}
	public:
		constexpr VkImageLayout getRefLayout() const override {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	};
}

#endif // !VAL_COLOR_ATTACHMENT_HPP
