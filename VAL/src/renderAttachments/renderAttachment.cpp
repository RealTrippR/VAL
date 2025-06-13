#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	VkAttachmentDescription renderAttachment::toVkAttachmentDescription() {
		VkAttachmentDescription desc{};
		desc.format = _imgFormat;
		desc.samples = VK_SAMPLE_COUNT_1_BIT;
		desc.loadOp = RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentLoadOp(_loadOp);
		desc.storeOp = RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentStoreOp(_storeOp);
		desc.stencilLoadOp = RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentLoadOp(_stencilLoadOp);
		desc.stencilStoreOp = RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentStoreOp(_stencilStoreOp);
		desc.initialLayout = _initialLayout;
		desc.finalLayout = _finalLayout;

		return desc;
	}

	void renderAttachment::setLoadOperation(const RENDER_ATTACHMENT_OPERATION& opType) {
		_loadOp = opType;
	}

	const RENDER_ATTACHMENT_OPERATION& renderAttachment::getLoadOperation() {
		return _loadOp;
	}

	void renderAttachment::setStoreOperation(const RENDER_ATTACHMENT_OPERATION& opType) {
#ifndef NDEBUG
		if (opType == RENDER_ATTACHMENT_OPERATION::Clear) {
			dbg::printError("Invalid store operation: RENDER_ATTACHMENT_OPERATION::Clear");
			throw std::logic_error("Invalid store operation: RENDER_ATTACHMENT_OPERATION::Clear");
		}
		if (opType == RENDER_ATTACHMENT_OPERATION::Discard) {
			dbg::printError("Invalid store operation: RENDER_ATTACHMENT_OPERATION::Discard");
			throw std::logic_error("Invalid store operation: RENDER_ATTACHMENT_OPERATION::Discard");
		}

#endif // !NDEBUG

		_storeOp = opType;
	}

	const RENDER_ATTACHMENT_OPERATION& renderAttachment::getStoreOperation() {
		return _storeOp;
	}

	void renderAttachment::setImgFormat(const VkFormat& format) {
		_imgFormat = format;
	}

	void renderAttachment::setInitialLayout(const VkImageLayout& l) {
		_initialLayout = l;
	}

	void renderAttachment::setFinalLayout(const VkImageLayout& l) {
		_finalLayout = l;
	}

	const VkFormat& renderAttachment::getImgFormat() {
		return _imgFormat;
	}

	const bool& renderAttachment::unused() {
		return _unused;
	}
}