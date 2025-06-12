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
INPUT(GraphicsPipeline& pipeline, Window& wind, VkCommandBuffer& cmd)
){
	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	FIXED_BEGIN(
		VkRenderPass pass;
		uint32_t subpassIndex;
	)		
	{
		static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };
		SET_PIPELINE(pipeline, V_PROC, cmd);

		SET_VERTEX_BUFFER(vertices, cmd);
		SET_INDEX_BUFFER(indices, cmd);


		SET_VIEWPORT(viewport, cmd);
		SET_SCISSOR(wind.getSize(), cmd);

	

		DRAW_INDEXED(indices.size(), cmd);
	}
	FIXED_END
}
PASS_END