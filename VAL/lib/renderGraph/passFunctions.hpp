/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VAL_PASS_FUNCTIONS_HPP
#define VAL_PASS_FUNCTIONS_HPP

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/renderGraph/passContext.hpp>
#include <VAL/lib/renderGraph/renderPassBeginType.hpp>

namespace val {

	inline void RESET_COMMAND_BUFFER(const VkCommandBuffer& cmd);

	inline void BEGIN_COMMAND_BUFFER(const VkCommandBuffer& cmd);

	inline void END_COMMAND_BUFFER(const VkCommandBuffer& cmd);

	inline void SUBMIT_COMMAND_BUFFER(VkCommandBuffer& cmd, PASS_CONTEXT& passContext, Queue& graphicsQueue, const VkFence& waitFence, const Queue& waitOnQueue);

	inline void BEGIN_RENDER_PASS(PASS_CONTEXT& passContext, GraphicsPipeline& pipeline, VkFramebuffer& framebuffer, const VkCommandBuffer& cmd, const val::RENDER_PASS_BEGIN_TYPE& beginType);

	inline void END_RENDER_PASS(const VkCommandBuffer& cmd);

	inline void SET_PIPELINE(ValProc& proc, GraphicsPipeline& pipeline, const VkCommandBuffer& commandBuffer);

	inline void SET_DESCRIPTOR_SET(ValProc& proc, GraphicsPipeline& pipeline, const VkCommandBuffer& commandBuffer);

	inline void SET_DESCRIPTOR_SET(ValProc& proc, GraphicsPipeline& pipeline, uint32_t setIndex, const VkCommandBuffer& commandBuffer);

	inline void SET_VIEWPORT(const VkViewport& viewport, const VkCommandBuffer& commandBuffer);

	inline void SET_SCISSOR(const VkRect2D& scissor, const VkCommandBuffer& commandBuffer);

	inline void SET_SCISSOR(const VkExtent2D& scissor, const VkCommandBuffer& commandBuffer);

	inline void TRANSITION_IMAGE_LAYOUT(VkDevice device, const ImageLayoutTransitionInfo imgInfo, 
		const IMAGE_LAYOUT layout, const IMAGE_ASPECT aspectMask, const PIPELINE_STAGE srcStageMask,
		const PIPELINE_STAGE dstStageMask, const ACCESS_FLAGS srcAccessMask, const ACCESS_FLAGS dstAccessMask,
		const DEPENDENCY_FLAGS dependencyFlags, const VkCommandBuffer& commandBuffer);
	//inline void setLineWidth();

	//inline void setBlendConstants();

	//inline void setTopologyMode();

	//inline void setCullMode();

	//inline void setDepthBias();

	//inline void copyImage(val::Image& dst, val::image& src);

	//inline void copyImage(val::image& dst, val::image& src, val::imageCopyInfo& copyInfo);

	//inline void copyBuffer(val::image& dst, val::image& src);

	inline void SET_VERTEX_BUFFER(VkBuffer& buffer, const VkCommandBuffer& commandBuffer, const VkDeviceSize& bufferOffset = 0u);

	//inline void setVertexBuffers();

	inline void SET_INDEX_BUFFER(VkBuffer& buffer, const VkCommandBuffer& commandBuffer);

	inline void DRAW_INSTANCED_INDEXED(val::Buffer& vertexBuffer, val::Buffer& indexBuffer, const uint32_t& instanceCount, const VkCommandBuffer& cmd,
		const uint32_t& firstIndex = 0u, const uint32_t& firstVertex = 0u, const uint32_t& firstInstance = 0u);

	inline void DRAW_INSTANCED(val::Buffer& vertexBuffer, const uint32_t& instanceCount, const VkCommandBuffer& cmd, const uint32_t& firstVertex = 0u, const uint32_t& firstInstance = 0u);

	inline void DRAW_INDEXED(const uint32_t& indexCount, const VkCommandBuffer& cmd, const uint32_t& firstIndex = 0u, const uint32_t& firstVertex = 0u);

	inline void DRAW(const uint32_t& vertexCount, const VkCommandBuffer& cmd, const uint32_t& firstVertex = 0u);
	//inline void setIndexAndVertexBuffers();
}

#endif // !VAL_PASS_FUNCTIONS_HPP