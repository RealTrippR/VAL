#ifndef VAL_COLOR_ATTACHMENT_HPP
#define VAL_COLOR_ATTACHMENT_HPP

#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	class colorAttachment : public renderAttachment
	{
	public:
		VkAttachmentDescription toVkAttachmentDescription() override {
			static VkAttachmentDescription desc;
			desc.format = _imgFormat;
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.loadOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VK_ATTACHMENT_LOAD_OP(_loadOp);
			desc.storeOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VK_ATTACHMENT_STORE_OP(_storeOp);
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

		void setImgFormat(const VkFormat& format) {
			_imgFormat = format;
		}

		const VkFormat& getImgFormat() {
			return _imgFormat;
		}

		static constexpr VkImageLayout getRefLayout() {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

	protected:
		VkFormat _imgFormat;
	};
}

#endif // !VAL_COLOR_ATTACHMENT_HPP
