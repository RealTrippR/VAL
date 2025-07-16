/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include "../vertex.hpp"
#include <VAL/lib/ext/gpu_vector.hpp>

using namespace val;

PASS_BEGIN(COLOR)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices),
WRITE(VkFramebuffer frameBuffer, Texture2D& renderTargImg),
INPUT(GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue)
){
	//renderTargImg.transitionLayout(graphicsQueue, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	TRANSITION_IMAGE_LAYOUT(valProc, renderTargImg, 
		IMAGE_LAYOUT::ColorAttachment,
		IMAGE_ASPECT::Color,
		PIPELINE_STAGE::TopOfPipe, PIPELINE_STAGE::ColorAttachmentOutput,
		ACCESS_FLAGS::None, ACCESS_FLAGS::ColorAttachmentWrite, 
		DEPENDENCY_FLAGS::None, graphicsQueue);

	static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };

	// it's important to note that BEGIN_RENDER_PASS and END_RENDER_PASS cannot be called inside fixed functions, 
	// the RG error handling must be improved to change this, also add error lines and reasons for failure
	BEGIN_RENDER_PASS(passContext, pipeline, frameBuffer, graphicsQueue, INLINE);

	SET_DESCRIPTOR_SET(valProc, pipeline, graphicsQueue);
	SET_PIPELINE(valProc, pipeline, graphicsQueue);

	SET_VERTEX_BUFFER(vertices, graphicsQueue);
	SET_INDEX_BUFFER(indices, graphicsQueue);

	SET_VIEWPORT(viewport, graphicsQueue);

	SET_SCISSOR(wind.getSize(), graphicsQueue);
	DRAW_INDEXED(indices.size(), graphicsQueue);

	END_RENDER_PASS(graphicsQueue);
}
PASS_END


PASS_BEGIN(IMAGE)
READ(gpu_vector<res::vertex>& vertices, gpu_vector<uint32_t>& indices, Texture2D& renderTargImg),
WRITE(VkFramebuffer frameBuffer),
INPUT(GraphicsPipeline pipeline, Window& wind, Queue& graphicsQueue)
){
	//renderTargImg.transitionLayout(graphicsQueue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	TRANSITION_IMAGE_LAYOUT(valProc, renderTargImg,
		IMAGE_LAYOUT::ShaderReadOnly,
		IMAGE_ASPECT::Color,
		PIPELINE_STAGE::TopOfPipe, PIPELINE_STAGE::FragmentShader,
		ACCESS_FLAGS::None, ACCESS_FLAGS::ShaderRead,
		DEPENDENCY_FLAGS::None, graphicsQueue);

	BEGIN_RENDER_PASS(passContext, pipeline, frameBuffer, graphicsQueue, INLINE);

	static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };

	SET_DESCRIPTOR_SET(valProc, pipeline, graphicsQueue);
	SET_PIPELINE(valProc, pipeline, graphicsQueue);

	SET_VERTEX_BUFFER(vertices, graphicsQueue);
	SET_INDEX_BUFFER(indices, graphicsQueue);

	SET_VIEWPORT(viewport, graphicsQueue);
	SET_SCISSOR(wind.getSize(), graphicsQueue);

	DRAW_INDEXED(indices.size(), graphicsQueue);

	END_RENDER_PASS(graphicsQueue);
}
PASS_END