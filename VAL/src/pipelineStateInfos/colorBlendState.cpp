#include <VAL/lib/pipelineStateInfos/colorBlendState.hpp>

namespace val
{
	void colorBlendState::bindBlendAttachment(val::colorBlendStateAttachment* attachment) {
		_attachments.push_back(attachment);
	}

	void colorBlendState::removeBlendAttachment(const uint32_t idx) {
		_attachments.erase(_attachments.begin() + idx);
	}

	std::vector<colorBlendStateAttachment*> colorBlendState::getBlendAttachments() {
		return _attachments;
	}

	void colorBlendState::setBlendConstants(float r, float g, float b, float a) {
		_blendConstantsColors[0] = r;
		_blendConstantsColors[1] = g;
		_blendConstantsColors[2] = b;
		_blendConstantsColors[3] = a;
	}

	void colorBlendState::setBlendConstants(const std::array<float, 4>& RGBA) {
		memcpy(_blendConstantsColors, RGBA.data(), RGBA.size());
	}

	glm::vec4 colorBlendState::getBlendConstants() {
		return {
			_blendConstantsColors[0],
			_blendConstantsColors[1],
			_blendConstantsColors[2],
			_blendConstantsColors[3]
		};
	}

	void colorBlendState::setLogicOpEnabled(const bool logicOpEnabled) {
		_logicOpEnabled = logicOpEnabled;
	}

	bool colorBlendState::getLogicOpEnabled() {
		return _logicOpEnabled;
	}

	void colorBlendState::setLogicOp(const VkLogicOp& logicOp, bool logicOpEnabled) {
		_logicOp = logicOp;
		_logicOpEnabled = logicOpEnabled;
	}

	void colorBlendState::setLogicOp(const VkLogicOp& logicOp) {
		_logicOp = logicOp;
	}
	const VkLogicOp& colorBlendState::getLogicOp() {
		return _logicOp;
	}


	VkPipelineColorBlendStateCreateInfo colorBlendState::toVkPipelineColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo* stateInfo, std::vector<VkPipelineColorBlendAttachmentState>* VKattachments)
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
		info.attachmentCount = VKattachments->size();
		info.pAttachments = VKattachments->data();
		memcpy(info.blendConstants, _blendConstantsColors, sizeof(info.blendConstants));
		info.logicOpEnable = _logicOpEnabled;
		info.logicOp = _logicOp;
		return info;
	}
}