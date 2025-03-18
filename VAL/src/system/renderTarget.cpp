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

	void renderTarget::updateViewports(VAL_PROC& proc, const std::vector<VkViewport>&viewports)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
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

	void renderTarget::update(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline, const std::vector<VkViewport>& viewports)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];

		vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);

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

		if (vkQueueSubmit(graphicsQueue._queue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		free(waitStages);
	}
}