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