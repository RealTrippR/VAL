#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val {
	void renderTarget::render(VAL_PROC& proc, const std::vector<VkViewport>& viewports, VkRenderPass& renderPass, VkFramebuffer& frameBuffer, const uint32_t& instanceCount /*DEFAULT = 1U*/)
	{
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];

		// this would make more sense to have in the update function
		_renderPassBeginInfo.renderPass = renderPass;
		_renderPassBeginInfo.framebuffer = frameBuffer;

		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
		vkCmdBeginRenderPass(commandBuffer, &_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


		if (_indexCount>0) {
			vkCmdDrawIndexed(commandBuffer, (uint32_t)(_indexCount), instanceCount, 0, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, instanceCount, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
		}

		vkCmdEndRenderPass(commandBuffer);
	}

	void renderTarget::update(VAL_PROC& proc, const uint16_t& pipelineIdx) {		

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
	}

	void renderTarget::begin(VAL_PROC& proc) {

		vkResetCommandBuffer(proc._graphicsQueue._commandBuffers[proc._currentFrame], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(proc._graphicsQueue._commandBuffers[proc._currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
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