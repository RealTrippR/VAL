#include <VAL/lib/pipelineStateInfos/colorBlendStateAttachment.hpp>

namespace val
{
	void ColorBlendStateAttachment::setBlendEnabled(bool blendEnabled) {
		_VKblendAttachment.blendEnable = blendEnabled;
	}

	bool ColorBlendStateAttachment::getBlendEnabled() {
		return _VKblendAttachment.blendEnable;
	}


	void ColorBlendStateAttachment::setColorWriteMask(const VkColorComponentFlags& writeMask) {
		_VKblendAttachment.colorWriteMask = writeMask;
	}

	VkColorComponentFlags ColorBlendStateAttachment::getColorWriteMask() {
		return _VKblendAttachment.colorWriteMask;
	}

	void ColorBlendStateAttachment::setColorOp(const VkBlendOp& op) {
		_VKblendAttachment.colorBlendOp = op;
	}

	const VkBlendOp& ColorBlendStateAttachment::getColorOp() {
		return _VKblendAttachment.colorBlendOp;
	}

	void ColorBlendStateAttachment::setAlphaOp(const VkBlendOp& op) {
		_VKblendAttachment.alphaBlendOp = op;
	}

	const VkBlendOp& ColorBlendStateAttachment::getAlphaOp() {
		return _VKblendAttachment.alphaBlendOp;
	}

	void ColorBlendStateAttachment::setBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend, const VkBlendFactor& alphaBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcColorBlendFactor = colorBlend;
			_VKblendAttachment.srcAlphaBlendFactor = alphaBlend;
		}
		else {
			_VKblendAttachment.dstColorBlendFactor = colorBlend;
			_VKblendAttachment.dstAlphaBlendFactor = alphaBlend;
		}
	}

	void ColorBlendStateAttachment::setColorBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcColorBlendFactor = colorBlend;
		}
		else {
			_VKblendAttachment.dstColorBlendFactor = colorBlend;
		}
	}

	void ColorBlendStateAttachment::setAlphaBlendFactor(const BLEND_POS& pos, const VkBlendFactor& alphaBlend) {
		if (pos == BLEND_POS::SOURCE) {
			_VKblendAttachment.srcAlphaBlendFactor = alphaBlend;
		}
		else {
			_VKblendAttachment.dstAlphaBlendFactor = alphaBlend;
		}
	}

	VkBlendFactor ColorBlendStateAttachment::getColorBlendFactor(const BLEND_POS& pos) {
		if (pos == BLEND_POS::SOURCE) {
			return _VKblendAttachment.srcColorBlendFactor;
		}
		else {
			return _VKblendAttachment.dstColorBlendFactor;
		}
	}

	VkBlendFactor ColorBlendStateAttachment::getAlphaBlendFactor(const BLEND_POS& pos) {
		if (pos == BLEND_POS::SOURCE) {
			return _VKblendAttachment.srcAlphaBlendFactor;
		}
		else {
			return _VKblendAttachment.dstAlphaBlendFactor;
		}
	}

	void ColorBlendStateAttachment::setFromVkPipelineColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& state) {
		_VKblendAttachment = state;
	}

	VkPipelineColorBlendAttachmentState& ColorBlendStateAttachment::getVkColorBlendAttachmentState() {
		return _VKblendAttachment;
	}
}