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