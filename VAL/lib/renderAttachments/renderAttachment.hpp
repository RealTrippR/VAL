#ifndef VAL_RENDER_ATTACHMENT_HPP
#define VAL_RENDER_ATTACHMENT_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/attachmentEnums.hpp>

namespace val {
	class subpass; // forward declaration

	enum RENDER_ATTACHMENT_USAGE {
		USED,
		UNUSED
	};

	class renderAttachment
	{
	public:
		renderAttachment() = default;
		renderAttachment(RENDER_ATTACHMENT_USAGE usage) {
			switch (usage)
			{
			case val::UNUSED:
				_unused = true;
				break;
			default:
				_unused = false;
				break;
			}
		}
	public:
		VkAttachmentDescription toVkAttachmentDescription();

		void setLoadOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType);
		
		const RENDER_ATTACHMENT_OPERATION_TYPE& getLoadOperation();

		void setStoreOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType);

		const RENDER_ATTACHMENT_OPERATION_TYPE& getStoreOperation();

		void setImgFormat(const VkFormat& format);

		void setInitialLayout(const VkImageLayout& l);

		void setFinalLayout(const VkImageLayout& l);

		const VkFormat& getImgFormat();

		const bool& unused();

		constexpr virtual VkImageLayout getRefLayout() const {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}

	protected:
		friend subpass;
		friend VAL_PROC;
		VkFormat _imgFormat = VK_FORMAT_UNDEFINED;
		RENDER_ATTACHMENT_OPERATION_TYPE _loadOp = CLEAR;
		RENDER_ATTACHMENT_OPERATION_TYPE _storeOp = STORE;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilLoadOp = DISCARD;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilStoreOp = DISCARD;
		VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		bool _unused = false;
	};
}
#endif // !VAL_RENDER_ATTACHMENT_HPP