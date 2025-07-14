#ifndef VAL_RESOLVE_ATTACHMENT_HPP
#define VAL_RESOLVE_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class ResolveAttachment : public renderAttachment 
	{
	public:
		ResolveAttachment() : renderAttachment() {}
		ResolveAttachment(RENDER_ATTACHMENT_USAGE usage) : renderAttachment(usage) {}
	public:
		constexpr VkImageLayout getRefLayout() const override {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	};
}

#endif // !VAL_RESOLVE_ATTACHMENT_HPP
