#include <VAL/lib/pipelineStateInfos/colorBlendState.hpp>

namespace val
{
	void ColorBlendState::bindBlendAttachment(val::ColorBlendStateAttachment* attachment) {
		_attachments.push_back(attachment);
	}

	void ColorBlendState::removeBlendAttachment(const uint32_t idx) {
		_attachments.erase(_attachments.begin() + idx);
	}

	std::vector<ColorBlendStateAttachment*> ColorBlendState::getBlendAttachments() {
		return _attachments;
	}

	void ColorBlendState::setBlendConstants(float r, float g, float b, float a) {
		_blendConstantsColors[0] = r;
		_blendConstantsColors[1] = g;
		_blendConstantsColors[2] = b;
		_blendConstantsColors[3] = a;
	}

	void ColorBlendState::setBlendConstants(const std::array<float, 4>& RGBA) {
		memcpy(_blendConstantsColors, RGBA.data(), RGBA.size());
	}

	glm::vec4 ColorBlendState::getBlendConstants() {
		return {
			_blendConstantsColors[0],
			_blendConstantsColors[1],
			_blendConstantsColors[2],
			_blendConstantsColors[3]
		};
	}

	void ColorBlendState::setLogicOpEnabled(const bool logicOpEnabled) {
		_logicOpEnabled = logicOpEnabled;
	}

	bool ColorBlendState::getLogicOpEnabled() {
		return _logicOpEnabled;
	}

	void ColorBlendState::setLogicOp(const VkLogicOp& logicOp, bool logicOpEnabled) {
		_logicOp = logicOp;
		_logicOpEnabled = logicOpEnabled;
	}

	void ColorBlendState::setLogicOp(const VkLogicOp& logicOp) {
		_logicOp = logicOp;
	}
	const VkLogicOp& ColorBlendState::getLogicOp() {
		return _logicOp;
	}


	VkPipelineColorBlendStateCreateInfo ColorBlendState::toVkPipelineColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo* stateInfo, std::vector<VkPipelineColorBlendAttachmentState>* VKattachments)
	{	
		if (_attachments.size() > 0) {
			for (auto& attachment : _attachments) {
				VKattachments->push_back(attachment->getVkColorBlendAttachmentState());
			}
		}
		VkPipelineColorBlendStateCreateInfo info;
		info.flags = VkPipelineColorBlendStateCreateFlags(0);
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		info.pNext = VK_NULL_HANDLE;
		info.attachmentCount = (uint32_t)VKattachments->size();
		info.pAttachments = VKattachments->data();
		memcpy(info.blendConstants, _blendConstantsColors, sizeof(info.blendConstants));
		info.logicOpEnable = _logicOpEnabled;
		info.logicOp = _logicOp;
		return info;
	}
}