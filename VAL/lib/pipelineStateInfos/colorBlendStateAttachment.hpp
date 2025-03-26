#ifndef VAL_COLOR_BLEND_STATE_HPP
#define VAL_COLOR_BLEND_STATE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class colorBlendStateAttachment
	{
	public:
		colorBlendStateAttachment() = default;
		colorBlendStateAttachment(bool blendEnabled) {
			_VKblendAttachment.blendEnable = blendEnabled;
		}
	public:

		void setBlendEnabled(bool blendEnabled);

		bool getBlendEnabled();

		void setColorWriteMask(const VkColorComponentFlags& writeMask);

		VkColorComponentFlags getColorWriteMask();

		void setColorOp(const VkBlendOp& op);

		const VkBlendOp& getColorOp();

		void setAlphaOp(const VkBlendOp& op);

		const VkBlendOp& getAlphaOp();

		void setBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend, const VkBlendFactor& alphaBlend);
		
		void setColorBlendFactor(const BLEND_POS& pos, const VkBlendFactor& colorBlend);

		void setAlphaBlendFactor(const BLEND_POS& pos, const VkBlendFactor& alphaBlend);

		VkBlendFactor getColorBlendFactor(const BLEND_POS& pos);

		VkBlendFactor getAlphaBlendFactor(const BLEND_POS& pos);

		void setFromVkPipelineColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& state);

		VkPipelineColorBlendAttachmentState& getVkColorBlendAttachmentState();

	protected:
		friend VAL_PROC;
		VkPipelineColorBlendAttachmentState _VKblendAttachment{};
	};
}

#endif // !VAL_COLOR_BLEND_STATE_HPP
