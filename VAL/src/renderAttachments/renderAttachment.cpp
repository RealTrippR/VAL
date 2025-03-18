#include <VAL/lib/renderAttachments/renderAttachment.hpp>

namespace val
{
	VkAttachmentDescription renderAttachment::toVkAttachmentDescription() {
		VkAttachmentDescription desc{};
		desc.format = _imgFormat;
		desc.samples = VkSampleCountFlagBits(_MSAA_Samples);
		desc.loadOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentLoadOp(_loadOp);
		desc.storeOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentStoreOp(_storeOp);
		desc.stencilLoadOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentLoadOp(_stencilLoadOp);
		desc.stencilStoreOp = RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentStoreOp(_stencilStoreOp);
		desc.initialLayout = _initialLayout;
		desc.finalLayout = _finalLayout;

		return desc;
	}

	uint8_t renderAttachment::getMSAA_Samples() {
		return uint8_t(_MSAA_Samples);
	}

	void renderAttachment::setMSAA_Samples(uint8_t sampleCount /*Must be a power of 2*/) {
#ifndef NDEBUG
		if ((sampleCount & (sampleCount - 1)) != 0) {
			printf("VAL: ERROR: Invalid sample count of %u; the sample count must be a power of two!\n", sampleCount);
			throw std::runtime_error("VAL: ERROR: Invalid sample count; the sample count must be a power of two!");

		}
#endif // !NDEBUG

		_MSAA_Samples = VkSampleCountFlags(sampleCount);
	}

	void renderAttachment::setLoadOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType) {
		_loadOp = opType;
	}

	const RENDER_ATTACHMENT_OPERATION_TYPE& renderAttachment::getLoadOperation() {
		return _loadOp;
	}

	void renderAttachment::setStoreOperation(const RENDER_ATTACHMENT_OPERATION_TYPE& opType) {
		_storeOp = opType;
	}

	const RENDER_ATTACHMENT_OPERATION_TYPE& renderAttachment::getStoreOperation() {
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