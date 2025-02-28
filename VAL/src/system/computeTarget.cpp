#include <VAL/lib/system/computeTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void computeTarget::compute(VAL_PROC& proc, queueManager& queue, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ)
	{
		const auto& currentFrame = proc._currentFrame;
		VkCommandBuffer& cmdBuffer = queue._commandBuffers[currentFrame];

		vkCmdDispatch(queue._commandBuffers[currentFrame], groupCountX, groupCountY, groupCountZ);

		if (vkEndCommandBuffer(queue._commandBuffers[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}

		//////////////////////////////////////////////////////////////
		/////  Compute submission /////
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &queue._commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &queue._semaphores[currentFrame];

		if (vkQueueSubmit(queue._queue, 1, &submitInfo, queue._fences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
	}

	void computeTarget::update(VAL_PROC& proc, queueManager& queue, computePipelineCreateInfo& computePipeline)
	{
		const auto& currentFrame = proc._currentFrame;
		VkCommandBuffer& cmdBuffer = queue._commandBuffers[currentFrame];

		vkResetFences(proc._device, 1, &queue._fences[currentFrame]);
		vkResetCommandBuffer(queue._commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);


		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, proc._computePipelines[computePipeline.pipelineIdx]);
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, proc._computePipelineLayouts[computePipeline.pipelineIdx],
			0, 1, &proc._descriptorSets[computePipeline.descriptorsIdx][currentFrame], 0, nullptr);


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


		if (vkBeginCommandBuffer(queue._commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording compute command buffer!");
		}
	}

	void computeTarget::submit(VAL_PROC& proc, const std::vector<queueManager&>& signalQueues,
		const std::vector<queueManager&>& waitQueues, VkFence fence /*DEFAULT=VK_NULL_HANDLE*/) {
		auto& currentFrame = proc._currentFrame;


		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


		// wait on the image available semaphore to ensure the graphicsQueue doesn't use a swap chain image before it's ready.
		//submitInfo.waitSemaphoreCount = syncInfo.waitSemaphores[currentFrame].size();
		//submitInfo.pWaitSemaphores = syncInfo.waitSemaphores[currentFrame].data();
		//pWaitDstStageMask = syncInfo.waitStages.data();
		// the length of pWaitDstStageMask should be that of waitSemaphoreCount
#ifndef NDEBUG
		/*if (submitInfo.waitSemaphoreCount != syncInfo.waitStages.size()) {
			printf("VAL: WARNING: val::syncInfo wait semaphore count and syncInfo.waitStages.size() do not match!\n");
		}*/
#endif // !NDEBUG

		/*
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &proc->_graphicsQueue._commandBuffers[_procVAL->_currentFrame];

		submitInfo.signalSemaphoreCount = signalSemaphores[currentFrame].size();
		submitInfo.pSignalSemaphores = signalSemaphores[currentFrame].data();

		if (vkQueueSubmit(_procVAL->_graphicsQueue._queue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER");
		}
		*/
	}
}