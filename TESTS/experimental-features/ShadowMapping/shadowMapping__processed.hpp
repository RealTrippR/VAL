#include <VAL/lib/system/VAL_PROC.hpp>
/********************************/
/* Copyright Tripp Robins, 2025 */
/********************************/
#include <VAL/lib/renderGraph/pass.hpp>

#include <VAL/lib/meshes&vertices/mesh.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr2D.hpp>

using namespace val;

void pass_mainSHADOW(val::ValProc& valProc, val::PASS_CONTEXT& passContext, Mesh<VertexTxtr, 1>& mesh, VkFramebuffer framebuffer, GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue) {


	BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, graphicsQueue, INLINE); // the INLINE/FIXED flag should be automatically set, this is bad code.

	static VkViewport viewport{ 0,0, wind.getWidth(), wind.getHeight(), 0.f, 1.f };
	SET_PIPELINE(valProc, pipeline, graphicsQueue);
	SET_DESCRIPTOR_SET(valProc, pipeline, graphicsQueue);

	SET_VERTEX_BUFFER(mesh.vertices, graphicsQueue);
	SET_INDEX_BUFFER(mesh.indices, graphicsQueue);


	SET_VIEWPORT(viewport, graphicsQueue);
	SET_SCISSOR(wind.getSize(), graphicsQueue);


	DRAW_INDEXED(mesh.indices.size(), graphicsQueue);

	END_RENDER_PASS(graphicsQueue);
}
void pass_mainMAIN(val::ValProc& valProc, val::PASS_CONTEXT& passContext, Mesh<VertexTxtr, 1>& mesh, VkFramebuffer framebuffer, GraphicsPipeline& pipeline, Window& wind, Queue& graphicsQueue) {

	//dbg::printWarning("TODO: Add validation to ensure that begin render pass has the correct INLINE/FIXED mode.");
	BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, graphicsQueue, INLINE); // the INLINE/FIXED flag should be automatically set, this is bad code.

	static VkViewport viewport{ 0,0, wind.getWidth(), wind.getHeight(), 0.f, 1.f};
	SET_PIPELINE(valProc, pipeline, graphicsQueue);
	SET_DESCRIPTOR_SET(valProc, pipeline, graphicsQueue);

	SET_VERTEX_BUFFER(mesh.vertices, graphicsQueue);
	SET_INDEX_BUFFER(mesh.indices, graphicsQueue);


	SET_VIEWPORT(viewport, graphicsQueue);
	SET_SCISSOR(wind.getSize(), graphicsQueue);


	DRAW_INDEXED(mesh.indices.size(), graphicsQueue);

	END_RENDER_PASS(graphicsQueue);
}
