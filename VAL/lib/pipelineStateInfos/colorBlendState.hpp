#ifndef VAL_COLOR_BLEND_STATE_HPP
#define VAL_COLOR_BLEND_STATE_HPP

#include <VAL/lib/pipelineStateInfos/colorBlendStateAttachment.hpp>

namespace val
{
	class colorBlendState
	{
	public:
		void bindBlendAttachment(val::colorBlendStateAttachment* attachment);

		void removeBlendAttachment(const uint32_t idx);

		std::vector<colorBlendStateAttachment*> getBlendAttachments();

		void setBlendConstants(float r, float g, float b, float a);

		void setBlendConstants(const std::array<float, 4>& RGBA);

		glm::vec4 getBlendConstants();

		void setLogicOpEnabled(const bool logicOpEnabled);

		bool getLogicOpEnabled();

		void setLogicOp(const VkLogicOp& logicOp, bool logicOpEnabled);

		void setLogicOp(const VkLogicOp& logicOp);

		const VkLogicOp& getLogicOp();

		VkPipelineColorBlendStateCreateInfo toVkPipelineColorblendStateCreateInfo(VkPipelineColorBlendStateCreateInfo* stateInfo,
			std::vector<VkPipelineColorBlendAttachmentState>* VKattachments);

	public:
		std::vector<colorBlendStateAttachment*> _attachments;
		float _blendConstantsColors[4] = { 0.f,0.f,0.f,0.f };
		bool _logicOpEnabled;
		VkLogicOp _logicOp;
	};
}

#endif //!colorBlendStateAttachment