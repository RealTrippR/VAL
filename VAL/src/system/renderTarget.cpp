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


#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val {
	void renderTarget::render(VAL_PROC& proc, const uint32_t& instanceCount /*DEFAULT = 1U*/)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];

		if (_indexCount>0) {
			vkCmdDrawIndexed(commandBuffer, (uint32_t)(_indexCount), instanceCount, 0, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, instanceCount, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
		}
	}

	void renderTarget::rebindDescriptorSet(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline) {
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
	}

	void renderTarget::updatePipeline(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
	}

	void renderTarget::updateViewport(VAL_PROC& proc, const VkViewport& viewport)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewport(VAL_PROC& proc, const VkViewport& viewport, const uint16_t index)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewports(VAL_PROC& proc, const std::vector<VkViewport>&viewports)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
	}

	void renderTarget::updateViewports(VAL_PROC& proc, const std::vector<VkViewport>& viewports, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, startIndex, viewports.size(), viewports.data());
	}

	void renderTarget::updateScissor(VAL_PROC& proc, const VkRect2D& scissor) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void renderTarget::updateScissor(VAL_PROC& proc, const VkRect2D& scissor, const uint16_t index) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetScissor(commandBuffer, index, 1, &scissor);
	}

	void renderTarget::updateScissors(VAL_PROC& proc, const std::vector<VkRect2D>& scissors) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetScissor(commandBuffer, 0, scissors.size(), scissors.data());
	}

	void renderTarget::updateScissors(VAL_PROC& proc, const std::vector<VkRect2D>& scissors, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetScissor(commandBuffer, startIndex, scissors.size(), scissors.data());
	}


	void renderTarget::updateLinewidth(VAL_PROC& proc, const float lineWidth) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetLineWidth(commandBuffer, lineWidth);
	}

	void renderTarget::updateBlendConstants(VAL_PROC& proc, const std::array<float, 4>& depthConstants) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetBlendConstants(commandBuffer, depthConstants.data());
	}

	void renderTarget::updateTopologyMode(VAL_PROC& proc, const TOPOLOGY_MODE topologyMode) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetPrimitiveTopology(commandBuffer,(VkPrimitiveTopology)topologyMode);
	}

	void renderTarget::updateCullMode(VAL_PROC& proc, const CULL_MODE cullMode) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetCullMode(commandBuffer, VkCullModeFlags(cullMode));
	}

	void renderTarget::updateDepthBias(VAL_PROC& proc, const float depthBiasConstant, const float depthBiasClamp, const float depthBiasSlopeFactor) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetDepthBias(commandBuffer, depthBiasConstant, depthBiasClamp, depthBiasSlopeFactor);
	}

	void renderTarget::updateBuffers(VAL_PROC& proc)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateIndexBuffer(VAL_PROC& proc) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateVertexBuffers(VAL_PROC& proc) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	/*****************************************************************************************************************************/
	/* BUFFER MANIPULATION*/

	void renderTarget::updateAndSetIndexBuffer(VAL_PROC& proc, val::buffer& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}

	void renderTarget::updateAndSetIndexBuffer(VAL_PROC& proc, const VkBuffer&& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}


	void renderTarget::updateAndSetVertexBuffer(VAL_PROC& proc, const VkBuffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffer(VAL_PROC& proc, val::buffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(VAL_PROC& proc, const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(VAL_PROC& proc, const std::vector<val::buffer&>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	/*****************************************************************************************************************************/

	void renderTarget::update(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline, const std::vector<VkViewport>& viewports)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];

		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);

		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount>0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
	}

	void renderTarget::beginPass(VAL_PROC& proc, VkRenderPass& renderPass, VkFramebuffer& frameBuffer) {
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];

		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}


		// this would make more sense to have in the update function
		_renderPassBeginInfo.renderPass = renderPass;
		_renderPassBeginInfo.framebuffer = frameBuffer;

		vkCmdBeginRenderPass(commandBuffer, &_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void renderTarget::endPass(VAL_PROC& proc) {
		vkCmdEndRenderPass(proc._graphicsQueue._commandBuffers[proc._currentFrame]);
	}

	
	void renderTarget::submit(VAL_PROC& proc,
		std::vector<VkSemaphore> waitSemaphores, VkFence fence /*DEFAULT=VK_NULL_HANDLE*/)
	{
		auto& graphicsQueue = proc._graphicsQueue;
		const auto& currentFrame = proc._currentFrame;

		// END RECORDING
		if (vkEndCommandBuffer(proc._graphicsQueue._commandBuffers[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER");
		}

		// SUBMIT
		VkPipelineStageFlags* waitStages = (VkPipelineStageFlags*)calloc(waitSemaphores.size(), sizeof(VkPipelineStageFlags));
		for (size_t i = 0; i < waitSemaphores.size(); ++i) {
			waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &graphicsQueue._commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &graphicsQueue._semaphores[currentFrame];

#ifndef NDEBUG
		if (vkQueueSubmit(graphicsQueue._queue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
#else
		vkQueueSubmit(graphicsQueue._queue, 1, &submitInfo, fence);
#endif // !NDEBUG

	

		free(waitStages);
	}
}