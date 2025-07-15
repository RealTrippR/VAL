#include <VAL/lib/system/VAL_PROC.hpp>
/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

/* A basic example rendergraph */
VkCommandBuffer __DRAW_RECT_fixed_cmd_buffer_0[2];
void pass_mainDRAW_RECT(val::ValProc& valProc, val::PASS_CONTEXT& passContext, gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, VkFramebuffer framebuffer, GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue) {

	BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, graphicsQueue, FIXED); // the INLINE/FIXED flag should be automatically set, this is bad code.

	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	FIXED_BEGIN(
		VkRenderPass pass;
		uint32_t subpassIndex;
	)	
		vkCmdExecuteCommands(graphicsQueue,1, &(__DRAW_RECT_fixed_cmd_buffer_0[valProc.getCurrentFrame()]));
FIXED_END

	END_RENDER_PASS(graphicsQueue);
}
void pass_bakeDRAW_RECT(val::ValProc&valProc, val::PASS_CONTEXT& passContext, gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, VkFramebuffer framebuffer, GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue, VkRenderPass pass,uint32_t subpassIndex) {

	; // the INLINE/FIXED flag should be automatically set, this is bad code.

	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	{
/*FIXED_BEGIN*/	
{

VkCommandBufferAllocateInfo allocInfo;
allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
allocInfo.pNext = VK_NULL_HANDLE;
allocInfo.commandPool = valProc._commandPool; 
allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
allocInfo.commandBufferCount = 2;
if (vkAllocateCommandBuffers(valProc._device, &allocInfo,__DRAW_RECT_fixed_cmd_buffer_0) != VK_SUCCESS) {
val::dbg::printError("Failed to allocate command buffers for baking render graph.");
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
		SET_PIPELINE(pipeline,valProc,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);

		SET_VERTEX_BUFFER(vertices,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
		SET_INDEX_BUFFER(indices,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);


		SET_VIEWPORT(viewport,__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
		SET_SCISSOR(wind.getSize(),__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);


		DRAW_INDEXED(indices.size(),__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
{
/* END RECORDING */
vkEndCommandBuffer(__DRAW_RECT_fixed_cmd_buffer_0[__current_frame_index__]);
}
}


	}
	
}/*FIXED_END*/

	;
}