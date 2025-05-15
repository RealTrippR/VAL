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

#ifndef VAL_DEPTH_STENCIL_STATE_HPP
#define VAL_DEPTH_STENCIL_STATE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class depthStencilState
	{
	public:

		void enableDepthTesting(bool enable);

		void enableStencilTesting(bool enable);

		void enableDepthBoundsTesting(bool enable);

		bool getDepthBoundsTesting();

		bool getStencilTesting();

		void setDepthBounds(float lowerBound, float upperBound);

		void setLowerDepthBound(float lowerBound);

		float getLowerDepthBound();

		void setUpperDepthBound(float upperBound);

		float getUpperDepthBound();

		void setCompareOp(VkCompareOp op);

		VkCompareOp getCompareOp();

		void setFront(const VkStencilOpState& front);

		VkStencilOpState& getFront();

		void setBack(const VkStencilOpState& back);

		VkStencilOpState& getBack();

		VkPipelineDepthStencilStateCreateInfo& getVkPipelineDepthStencilStateCreateInfo();
	protected:
		friend VAL_PROC;
		VkPipelineDepthStencilStateCreateInfo _VKdepthState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = NULL,
			.depthTestEnable = true,
			.depthWriteEnable = true,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.front = {},
			.back = {}
		};
	};
}

#endif // !VAL_DEPTH_STENCIL_STATE_HPP