/* Copyright Tripp Robins, 2025 */
#include <VAL/lib/renderGraph/pass.hpp>
/*************************************/

#include <stdio.h>
#include <utility>
// the compiler will have to give it the address of the VAL_PROC and set the pass_proc var.

using namespace val;

/* A basic example rendergraph */
PASS_BEGIN(DRAW_RECT)
READ(buffer& vertexBuffer, buffer& indexBuffer)
//WRITE(NULL)
//READ_WRITE(NULL)
//FIXED
INPUT(graphicsPipelineCreateInfo& pipeline, window& wind, const std::pair<int,int> i = { 1,2 })
{
	/*Consider adding a FIXED flag to "bake" the command buffers, 
	that way they won't have to be redone every frame,
	but can not be changed at runtime*/

	static VkViewport viewport{ 0,0, wind.getSize().width, wind.getSize().height, 0.f, 1.f };

	setPipeline(pipeline);

	setViewport(viewport);
	setScissor(wind.getSize()); // /* 

	setIndexBuffer(indexBuffer);
	setVertexBuffer(vertexBuffer);

}
PASS_END