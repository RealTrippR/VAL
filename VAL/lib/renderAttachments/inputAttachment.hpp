#ifndef VAL_INPUT_ATTACHMENT_HPP
#define VAL_INPUT_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class inputAttachment : public renderAttachment {


	protected:
		static constexpr VkImageLayout _layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	};
}

#endif // !VAL_INPUT_ATTACHMENT_HPP
