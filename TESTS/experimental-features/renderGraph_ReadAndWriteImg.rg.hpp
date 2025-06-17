/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

PASS_BEGIN(COLOR_PIPELINE)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices),
WRITE(VkFramebuffer frameBuffer, Image& renderTargImg),
INPUT(GraphicsPipeline& pipeline, Window& wind, VkCommandBuffer& cmd)
){
	renderTargImg.transitionImgLayout(valProc, cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };

	// it's important to note that BEGIN_RENDER_PASS and END_RENDER_PASS cannot be called inside fixed functions, 
	// the RG error handling must be improved to change this, also add error lines and reasons for failure
	BEGIN_RENDER_PASS(passContext, pipeline, frameBuffer, cmd, INLINE);

	SET_PIPELINE(pipeline, valProc, cmd);

	SET_VERTEX_BUFFER(vertices, cmd);
	SET_INDEX_BUFFER(indices, cmd);

	SET_VIEWPORT(viewport, cmd);

	SET_SCISSOR(wind.getSize(), cmd);

	DRAW_INDEXED(indices.size(), cmd);

	END_RENDER_PASS(cmd);
}
PASS_END


PASS_BEGIN(IMAGE_PIPELINE)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices),
INPUT(GraphicsPipeline pipeline, Window& wind, VkCommandBuffer& cmd, Image& renderTargImg, )
){

	renderTargImg.transitionImgLayout(valProc, cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	BEGIN_RENDER_PASS();

	static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };

	SET_PIPELINE(pipeline, valProc, cmd);

	SET_VERTEX_BUFFER(vertices, cmd);
	SET_INDEX_BUFFER(indices, cmd);

	SET_VIEWPORT(viewport, cmd);

	SET_SCISSOR(wind.getSize(), cmd);

	END_RENDER_PASS();
}