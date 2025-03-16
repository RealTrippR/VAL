#ifndef VAL_COLOR_ATTACHMENT_HPP
#define VAL_COLOR_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class colorAttachment : public renderAttachment
	{
	public:

		constexpr VkImageLayout getRefLayout() const override {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

	protected:
		VkFormat _imgFormat;
	};
}

#endif // !VAL_COLOR_ATTACHMENT_HPP
