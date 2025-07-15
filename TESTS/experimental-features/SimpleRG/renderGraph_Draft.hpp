/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

/* A basic example rendergraph */
PASS_BEGIN(DRAW_RECT)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices),
WRITE(VkFramebuffer framebuffer),
//READ_WRITE(NULL) <- upon further research I believe that this is invalid
INPUT(GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue)
){
	BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, graphicsQueue, FIXED); // the INLINE/FIXED flag should be automatically set, this is bad code.

	// a fixed subroutine  https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
	FIXED_BEGIN(
		VkRenderPass pass;
		uint32_t subpassIndex;
	)		
	{

		static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };
		SET_PIPELINE(pipeline, valProc, graphicsQueue);

		SET_VERTEX_BUFFER(vertices, graphicsQueue);
		SET_INDEX_BUFFER(indices, graphicsQueue);


		SET_VIEWPORT(viewport, graphicsQueue);
		SET_SCISSOR(wind.getSize(), graphicsQueue);


		DRAW_INDEXED(indices.size(), graphicsQueue);

	}
	FIXED_END

	END_RENDER_PASS(graphicsQueue);
}
PASS_END