#include <VAL/lib/pipelineStateInfos/colorBlendStateAttachment.hpp>

namespace val
{
	void colorBlendStateAttachment::setBlendEnabled(bool blendEnabled) {
		_VKblendAttachment.blendEnable = blendEnabled;
	}

	bool colorBlendStateAttachment::getBlendEnabled() {
		return _VKblendAttachment.blendEnable;
	}


	void colorBlendStateAttachment::setColorWriteMask(const VkColorComponentFlags& writeMask) {
		_VKblendAttachment.colorWriteMask = writeMask;
	}

	VkColorComponentFlags colorBlendStateAttachment::getColorWriteMask() {
		return _VKblendAttachment.colorWriteMask;
	}

	void colorBlendStateAttachment::setColorOp(const VkBlendOp& op) {
		_VKblendAttachment.colorBlendOp = op;
	}

	const VkBlendOp& colorBlendStateAttachment::getColorOp() {
		return _VKblendAttachment.colorBlendOp;
	}

	void colorBlendStateAttachment::setAlphaOp(const VkBlendOp& op) {
		_VKblendAttachment.alphaBlendOp = op;
	}

	const VkBlendOp& colorBlendStateAttachment::getAlphaOp() {
		return _VKblendAttachment.alphaBlendOp;
	}

	void colorBlendStateAttachment::setBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend, const VkBlendFactor& alphaBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcColorBlendFactor = colorBlend;
			_VKblendAttachment.srcAlphaBlendFactor = alphaBlend;
		}
		else {
			_VKblendAttachment.dstColorBlendFactor = colorBlend;
			_VKblendAttachment.dstAlphaBlendFactor = alphaBlend;
		}
	}

	void colorBlendStateAttachment::setColorBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcColorBlendFactor = colorBlend;
		}
		else {
			_VKblendAttachment.dstColorBlendFactor = colorBlend;
		}
	}

	void colorBlendStateAttachment::setAlphaBlendFactor(const BLEND_POS& pos, const VkBlendFactor& alphaBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcAlphaBlendFactor = alphaBlend;
		}
		else {
			_VKblendAttachment.dstAlphaBlendFactor = alphaBlend;
		}
	}

	VkBlendFactor colorBlendStateAttachment::getColorBlendFactor(const BLEND_POS& pos) {
		if (pos == BLEND_POS::SOURCE) {
			return _VKblendAttachment.srcColorBlendFactor;
		}
		else {
			return _VKblendAttachment.dstColorBlendFactor;
		}
	}

	VkBlendFactor colorBlendStateAttachment::getAlphaBlendFactor(const BLEND_POS& pos) {
		if (pos == BLEND_POS::SOURCE) {
			return _VKblendAttachment.srcAlphaBlendFactor;
		}
		else {
			return _VKblendAttachment.dstAlphaBlendFactor;
		}
	}

	void colorBlendStateAttachment::setFromVkPipelineColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& state) {
		_VKblendAttachment = state;
	}

	VkPipelineColorBlendAttachmentState& colorBlendStateAttachment::getVkColorBlendAttachmentState() {
		return _VKblendAttachment;
	}
}