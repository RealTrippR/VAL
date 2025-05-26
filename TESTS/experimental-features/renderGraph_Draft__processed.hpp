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
VkCommandBuffer __DRAW_RECT_fixed_cmd_buffer_0;
void pass_mainDRAW_RECT(val::VAL_PROC& V_PROC,gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, graphicsPipelineCreateInfo& pipeline, window& wind, VkCommandBuffer& cmd) {

	/*Consider adding a FIXED flag to "bake" the command buffers, 
	that way they won't have to be redone every frame,
	but can not be changed at runtime*/
	
	// a fixed subroutine
	FIXED_BEGIN 
		vkCmdExecuteCommands(cmd,1, &(__DRAW_RECT_fixed_cmd_buffer_0));
FIXED_END
}
void pass_bakeDRAW_RECT(val::VAL_PROC& V_PROC,gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, graphicsPipelineCreateInfo& pipeline, window& wind, VkCommandBuffer& cmd) {

	/*Consider adding a FIXED flag to "bake" the command buffers, 
	that way they won't have to be redone every frame,
	but can not be changed at runtime*/
	
	// a fixed subroutine
	FIXED_BEGIN 
{

VkCommandBufferAllocateInfo allocInfo;
allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
allocInfo.commandPool = V_PROC._commandPool;
allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
allocInfo.commandBufferCount = 1;
if (vkAllocateCommandBuffers(V_PROC._device, &allocInfo, &__DRAW_RECT_fixed_cmd_buffer_0) != VK_SUCCESS) {throw std::runtime_error("Failed to allocate command buffers!");}
}

{
VkCommandBufferBeginInfo beginInfo{}; beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
vkBeginCommandBuffer(__DRAW_RECT_fixed_cmd_buffer_0, &beginInfo);
}
// https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	{
		setPipeline(pipeline,V_PROC,__DRAW_RECT_fixed_cmd_buffer_0);

		setVertexBuffer(vertices,__DRAW_RECT_fixed_cmd_buffer_0);
		setIndexBuffer(indices,__DRAW_RECT_fixed_cmd_buffer_0);


		static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };
		setViewport(viewport,__DRAW_RECT_fixed_cmd_buffer_0);
		setScissor(wind.getSize(),__DRAW_RECT_fixed_cmd_buffer_0);

	

		drawIndexed(indices.size(), cmd);
{
vkEndCommandBuffer(__DRAW_RECT_fixed_cmd_buffer_0);
}

	}
	FIXED_END
}