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

#ifndef VAL_PASS_FUNCTION_DEFINITIONS_HPP
#define VAL_PASS_FUNCTION__DEFINITIONS_HPP

#include <VAL/lib/renderGraph/passFunctions.hpp>


namespace val {

	inline void RESET_COMMAND_BUFFER(VkCommandBuffer& cmd) 
	{
		vkResetCommandBuffer(cmd, 0);
	}

	inline void BEGIN_COMMAND_BUFFER(VkCommandBuffer& cmd) 
	{
		thread_local static VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = VK_NULL_HANDLE;
		beginInfo.flags = 0u;
		beginInfo.pInheritanceInfo = VK_NULL_HANDLE;

		if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
	}

	inline void END_COMMAND_BUFFER(VkCommandBuffer& cmd) 
	{
		vkEndCommandBuffer(cmd);
	}

	inline void BEGIN_RENDER_PASS(PASS_CONTEXT& passContext, graphicsPipelineCreateInfo& pipeline, VkFramebuffer& framebuffer, VkCommandBuffer& cmd) 
	{

		VkRenderPassBeginInfo renderPassBeginInfo{ 
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, 
			NULL, 
			pipeline.getVkRenderPass(),
			framebuffer,
			passContext.renderArea,
			passContext.clearValues.size(),
			passContext.clearValues.data()
		};
		vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	inline void END_RENDER_PASS(VkCommandBuffer& cmd) {
		vkCmdEndRenderPass(cmd);
	}

	inline void setPipeline(graphicsPipelineCreateInfo& pipeline, VAL_PROC& proc, VkCommandBuffer& commandBuffer) {
		const auto& pipelineIdx = pipeline.pipelineIdx;
		//VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipeline(proc));
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
	}

	inline void setViewport(const VkViewport& viewport, VkCommandBuffer& commandBuffer) {
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	inline void setScissor(const VkRect2D& scissor, VkCommandBuffer& commandBuffer) {
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	inline void setScissor(const VkExtent2D& scissor, VkCommandBuffer& commandBuffer) {
		const VkRect2D _scissor = { 0, 0, scissor.width, scissor.height };
		vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);
	}

	inline void setVertexBuffer(VkBuffer& buffer, VkCommandBuffer& commandBuffer, const VkDeviceSize& bufferOffset) {
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &bufferOffset);
	}

	inline void setIndexBuffer(VkBuffer& buffer, VkCommandBuffer& commandBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	inline void drawInstanced(val::buffer& vertexBuffer, val::buffer& indexBuffer, const uint32_t& instanceCount, 
		VkCommandBuffer& cmd, const uint32_t& firstIndex, const uint32_t& firstVertex, const uint32_t& firstInstance)
	{
		vkCmdDrawIndexed(cmd, indexBuffer.size(), instanceCount, firstIndex, firstVertex, firstInstance);
	}

	inline void drawInstanced(val::buffer& vertexBuffer, const uint32_t& instanceCount,
		VkCommandBuffer& cmd, const uint32_t& firstInstance, const uint32_t& firstVertex)
	{
		vkCmdDraw(cmd, vertexBuffer.size(), instanceCount, firstVertex, firstInstance);
	}

	inline void drawIndexed(const uint32_t& indexCount, VkCommandBuffer& cmd, const uint32_t& firstIndex, const uint32_t& firstVertex)
	{
		vkCmdDrawIndexed(cmd, indexCount, 1, firstIndex, firstVertex, 0);
	}

	inline void draw(const uint32_t& vertexCount,VkCommandBuffer& cmd, const uint32_t& firstVertex)
	{
		vkCmdDraw(cmd, vertexCount, 1, 0, 0);
	}
}

#endif // !VAL_PASS_FUNCTION_DEFINITIONS_HPP