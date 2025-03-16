#ifndef VAL_RENDER_ATTACHMENT_HPP
#define VAL_RENDER_ATTACHMENT_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/attachmentEnums.hpp>

namespace val {
	class renderAttachment
	{
	public:
		VkAttachmentDescription toVkAttachmentDescription();

		uint8_t getMSAA_Samples();

		void setMSAA_Samples(uint8_t sampleCount /*Must be a power of 2*/);

		void setLoadOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType);
		
		const RENDER_ATTACHMENT_OPERATION_TYPE& getLoadOperation();

		void setStoreOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType);

		const RENDER_ATTACHMENT_OPERATION_TYPE& getStoreOperation();

		void setImgFormat(const VkFormat& format);

		void setInitialLayout(const VkImageLayout& l);

		void setFinalLayout(const VkImageLayout& l);

		const VkFormat& getImgFormat();

		constexpr virtual VkImageLayout getRefLayout() const {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}

	protected:
		VkFormat _imgFormat = VK_FORMAT_UNDEFINED;
		uint8_t _MSAA_Samples = 1u;
		RENDER_ATTACHMENT_OPERATION_TYPE _loadOp = CLEAR;
		RENDER_ATTACHMENT_OPERATION_TYPE _storeOp = STORE;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilLoadOp = DISCARD;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilStoreOp = DISCARD;
		VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	};
}
#endif // !VAL_RENDER_ATTACHMENT_HPP