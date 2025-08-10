#ifndef VAL_RENDER_ATTACHMENT_HPP
#define VAL_RENDER_ATTACHMENT_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/attachmentEnums.hpp>

namespace val {
	class Subpass; // forward declaration

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

		void setLoadOperation(const RENDER_ATTACHMENT_OPERATION& opType);
		
		const RENDER_ATTACHMENT_OPERATION& getLoadOperation();

		void setStoreOperation(const RENDER_ATTACHMENT_OPERATION& opType);

		const RENDER_ATTACHMENT_OPERATION& getStoreOperation();

		void setImgFormat(const VkFormat format);

		void setInitialLayout(const VkImageLayout l);

		void setFinalLayout(const VkImageLayout l);

		void setInitialLayout(const IMAGE_LAYOUT l);

		void setFinalLayout(const IMAGE_LAYOUT l);

		const VkFormat& getImgFormat() const ;

		const bool& unused();

		constexpr virtual VkImageLayout getRefLayout() const {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}

	protected:
		friend Subpass;
		friend ValProc;
		VkFormat _imgFormat = VK_FORMAT_UNDEFINED;
		RENDER_ATTACHMENT_OPERATION _loadOp = RENDER_ATTACHMENT_OPERATION::Clear;
		RENDER_ATTACHMENT_OPERATION _storeOp = RENDER_ATTACHMENT_OPERATION::Store;
		RENDER_ATTACHMENT_OPERATION _stencilLoadOp = RENDER_ATTACHMENT_OPERATION::Discard;
		RENDER_ATTACHMENT_OPERATION _stencilStoreOp = RENDER_ATTACHMENT_OPERATION::Discard;
		VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		bool _unused = false;
	};
}
#endif // !VAL_RENDER_ATTACHMENT_HPP