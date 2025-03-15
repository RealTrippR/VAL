#ifndef VAL_DEPTH_ATTACHMENT_HPP
#define VAL_DEPTH_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class depthAttachment : public renderAttachment {


	protected:
		static constexpr VkImageLayout _layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	};
}

#endif // !VAL_DEPTH_ATTACHMENT_HPP
