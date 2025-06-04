/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VAL_COLOR_BLEND_STATE_ATTACHMENT_HPP
#define VAL_COLOR_BLEND_STATE_ATTACHMENT_HPP

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

#endif // !VAL_COLOR_BLEND_STATE_ATTACHMENT_HPP
