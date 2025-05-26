/* Copyright Tripp Robins, 2025 */
#include <VAL/lib/renderGraph/pass.hpp>
/*************************************/


#include <stdio.h>
#include <utility>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

/* A basic example rendergraph */
PASS_BEGIN(DRAW_RECT)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices)
//WRITE(NULL)
//READ_WRITE(NULL)
INPUT(graphicsPipelineCreateInfo& pipeline, window& wind, VkCommandBuffer& cmd)
){
	/*Consider adding a FIXED flag to "bake" the command buffers, 
	that way they won't have to be redone every frame,
	but can not be changed at runtime*/
	
	// a fixed subroutine
	FIXED_BEGIN // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	{
		setPipeline(pipeline, V_PROC, cmd);

		setVertexBuffer(vertices, cmd);
		setIndexBuffer(indices, cmd);


		static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };
		setViewport(viewport, cmd);
		setScissor(wind.getSize(), cmd);

	

		drawIndexed(indices.size(), cmd);
	}
	FIXED_END
}
PASS_END