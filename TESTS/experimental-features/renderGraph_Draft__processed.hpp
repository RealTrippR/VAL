#include <VAL/lib/system/VAL_PROC.hpp>
/* Copyright Tripp Robins, 2025 */
#include <VAL/lib/renderGraph/pass.hpp>
/*************************************/


#include <stdio.h>
#include <utility>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

/* A basic example rendergraph */
VkCommandBuffer __DRAW_RECT_fixed_cmd_buffer_0[2];
void pass_mainDRAW_RECT(val::VAL_PROC& V_PROC,gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, graphicsPipelineCreateInfo& pipeline, window& wind, VkCommandBuffer& cmd) {

	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	FIXED_BEGIN(
		VkRenderPass pass;
		uint32_t subpassIndex;
	)	
		vkCmdExecuteCommands(cmd,1, &(__DRAW_RECT_fixed_cmd_buffer_0[V_PROC.getCurrentFrame()]));
FIXED_END
}
void pass_bakeDRAW_RECT(val::VAL_PROC& V_PROC,gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, graphicsPipelineCreateInfo& pipeline, window& wind, VkCommandBuffer& cmd, VkRenderPass pass,uint32_t subpassIndex) {

	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	{	
{

VkCommandBufferAllocateInfo allocInfo;
allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
allocInfo.pNext = VK_NULL_HANDLE;
allocInfo.commandPool = V_PROC._commandPool;
allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
allocInfo.commandBufferCount = 2;
if (vkAllocateCommandBuffers(V_PROC._device, &allocInfo,__DRAW_RECT_fixed_cmd_buffer_0) != VK_SUCCESS) {
throw std::runtime_error("Failed to allocate command buffers!");
}

}
for (uint8_t __current_frame_index__ = 0;__current_frame_index__ < 2;++__current_frame_index__) { 

{
/* BEGIN RECORDING */
VkCommandBufferInheritanceInfo inheritanceInfo = {
.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
.renderPass = pass,
.subpass = subpassIndex};
VkCommandBufferBeginInfo beginInfo{};
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
beginInfo.pInheritanceInfo = &inheritanceInfo;
beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;  // Or 0 if outside render pass

vkBeginCommandBuffer(__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__], &beginInfo);

}
	
	{
		static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };
		setPipeline(pipeline,V_PROC,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);

		setVertexBuffer(vertices,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
		setIndexBuffer(indices,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);


		setViewport(viewport,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
		setScissor(wind.getSize(),__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);

	

		drawIndexed(indices.size(),__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
{
/* END RECORDING */
vkEndCommandBuffer(__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
}
}

	}
	}
}