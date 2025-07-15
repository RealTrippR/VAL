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

	inline void RESET_COMMAND_BUFFER(const VkCommandBuffer& cmd)
	{
		vkResetCommandBuffer(cmd, 0);
	}

	inline void BEGIN_COMMAND_BUFFER(const VkCommandBuffer& cmd)
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

	inline void END_COMMAND_BUFFER(const VkCommandBuffer& cmd)
	{
		vkEndCommandBuffer(cmd);
	}

	inline void SUBMIT_COMMAND_BUFFER(VkCommandBuffer& cmd, PASS_CONTEXT& passContext, Queue& queue, const VkFence& waitFence, const Queue& waitOnQueue)
	{
		auto& proc = passContext.proc;
		const auto& currentFrame = proc._currentFrame;

		VkPipelineStageFlags waitStages = passContext.getWaitStages();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1u;
		submitInfo.pWaitSemaphores = &(waitOnQueue.getSemaphore());
		submitInfo.pWaitDstStageMask = &waitStages; // 1 wait stage for every semaphor.

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &queue.getCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &queue.getSemaphore();

#ifndef NDEBUG
		if (vkQueueSubmit(queue.getVkQueue(), 1, &submitInfo, waitFence) != VK_SUCCESS) {
			dbg::printError("Failed to submit command buffer @ %p to graphics queue.", &queue.getCommandBuffer());
			throw std::runtime_error("failed to submit draw command buffer!");
		}
#else
		vkQueueSubmit(queue._queue, 1, &submitInfo, waitFence);
#endif // !NDEBUG
	}


	inline void BEGIN_RENDER_PASS(PASS_CONTEXT& passContext, GraphicsPipeline& pipeline, VkFramebuffer& framebuffer, const VkCommandBuffer& cmd, const val::RENDER_PASS_BEGIN_TYPE& beginType)
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
		vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VkSubpassContents(beginType));
	}

	inline void END_RENDER_PASS(const VkCommandBuffer& cmd) {
		vkCmdEndRenderPass(cmd);
	}

	inline void SET_PIPELINE(GraphicsPipeline& pipeline, ValProc& proc, const VkCommandBuffer& commandBuffer) {
		const auto& pipelineIdx = pipeline.pipelineIdx;
		//VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipeline(proc));
	}

	inline void SET_DESCRIPTOR_SET(GraphicsPipeline& pipeline, ValProc& proc, const VkCommandBuffer& commandBuffer)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelineLayouts[pipelineIdx],
			0, 1, &(pipeline.getDescriptorSheet()->getVkDescriptorSets()[proc.getCurrentFrame()]), 0, nullptr);
	}

	inline void SET_DESCRIPTOR_SET(GraphicsPipeline& pipeline, ValProc& proc, uint32_t setIndex, const VkCommandBuffer& commandBuffer)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelineLayouts[pipelineIdx],
			0, 1, &(pipeline.getDescriptorSheet()->getVkDescriptorSets()[setIndex]), 0, nullptr);
	}

	inline void SET_VIEWPORT(const VkViewport& viewport, const VkCommandBuffer& commandBuffer) {
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	inline void SET_SCISSOR(const VkRect2D& scissor, const VkCommandBuffer& commandBuffer) {
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	inline void SET_SCISSOR(const VkExtent2D& scissor, const VkCommandBuffer& commandBuffer) {
		const VkRect2D _scissor = { 0, 0, scissor.width, scissor.height };
		vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);
	}

	inline void TRANSITION_IMAGE_LAYOUT(const ImageLayoutTransitionInfo& imgInfo, const IMAGE_LAYOUT layout, const VkCommandBuffer& commandBuffer)
	{
		
	}

	inline void SET_VERTEX_BUFFER(VkBuffer& buffer, const VkCommandBuffer& commandBuffer, const VkDeviceSize& bufferOffset) {
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &bufferOffset);
	}

	inline void SET_INDEX_BUFFER(VkBuffer& buffer, const VkCommandBuffer& commandBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	inline void DRAW_INSTANCED_INDEXED(val::Buffer& vertexBuffer, val::Buffer& indexBuffer, const uint32_t& instanceCount,
		const VkCommandBuffer& cmd, const uint32_t& firstIndex, const uint32_t& firstVertex, const uint32_t& firstInstance)
	{
		vkCmdDrawIndexed(cmd, indexBuffer.size(), instanceCount, firstIndex, firstVertex, firstInstance);
	}

	inline void DRAW_INSTANCED(val::Buffer& vertexBuffer, const uint32_t& instanceCount,
		const VkCommandBuffer& cmd, const uint32_t& firstInstance, const uint32_t& firstVertex)
	{
		vkCmdDraw(cmd, vertexBuffer.size(), instanceCount, firstVertex, firstInstance);
	}

	inline void DRAW_INDEXED(const uint32_t& indexCount, const VkCommandBuffer& cmd, const uint32_t& firstIndex, const uint32_t& firstVertex)
	{
		vkCmdDrawIndexed(cmd, indexCount, 1, firstIndex, firstVertex, 0);
	}

	inline void DRAW(const uint32_t& vertexCount, const VkCommandBuffer& cmd, const uint32_t& firstVertex)
	{
		vkCmdDraw(cmd, vertexCount, 1, 0, 0);
	}
}

#endif // !VAL_PASS_FUNCTION_DEFINITIONS_HPP