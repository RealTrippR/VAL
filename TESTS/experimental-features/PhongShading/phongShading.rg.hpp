/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include <VAL/lib/meshes&vertices/mesh.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr2D.hpp>

using namespace val;

/* A basic example rendergraph */
PASS_BEGIN(PHONG)

READ(Mesh<VertexTxtr,1>& mesh),
WRITE(VkFramebuffer framebuffer),
//READ_WRITE(NULL) <- upon further research I believe that this is invalid
INPUT(GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue)
){
	//dbg::printWarning("TODO: Add validation to ensure that begin render pass has the correct INLINE/FIXED mode.");
	BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, graphicsQueue, INLINE); // the INLINE/FIXED flag should be automatically set, this is bad code.

	static VkViewport viewport{ 0,0, wind.getWidth(), wind.getHeight(), 0.f, 1.f};
	SET_PIPELINE(pipeline, valProc, graphicsQueue);
	SET_DESCRIPTOR_SET(pipeline, valProc, graphicsQueue);

	SET_VERTEX_BUFFER(mesh.vertices, graphicsQueue);
	SET_INDEX_BUFFER(mesh.indices, graphicsQueue);


	SET_VIEWPORT(viewport, graphicsQueue);
	SET_SCISSOR(wind.getSize(), graphicsQueue);


	DRAW_INDEXED(mesh.indices.size(), graphicsQueue);

	END_RENDER_PASS(graphicsQueue);
}

PASS_END // PHONG END