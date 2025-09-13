#include <VAL/lib/system/computeTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void computeTarget::compute(Queue& computeQueue, VkCommandBuffer cmdBuffer, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ)
	{
		vkCmdDispatch(cmdBuffer, groupCountX, groupCountY, groupCountZ);
	}

	void computeTarget::update(Queue& computeQueue, VkCommandBuffer cmdBuffer, computePipelineCreateInfo& computePipeline)
	{
		ValProc& proc = *computeQueue.getValProc();
		const auto& currentFrame = proc.getCurrentFrame();
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, proc._computePipelines[computePipeline.pipelineIdx]);
	}

	void computeTarget::begin(Queue& queue)
	{
		const auto& currentFrame = queue.getValProc()->getCurrentFrame();

		// wait for the previosly submitted compute command buffer to finish
		//vkWaitForFences(queue.getValProc()->getVkLogicalDevice(), 1, &queue._fences[proc._currentFrame], VK_TRUE, UINT64_MAX);
		//vkResetFences(queue.getValProc()->getVkLogicalDevice(), 1, &queue._fences[proc._currentFrame]);
		//vkResetCommandBuffer(queue._commandBuffers[proc._currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

		/*VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(queue._commandBuffers[proc._currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}*/

		//queue
	}

	void computeTarget::submit(Queue& computeQueue, VkPipelineStageFlags* waitStages, VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount,
		uint32_t commandBufferCount, VkSemaphore* waitSemaphores, uint32_t waitSemaphoreCount,
		VkSemaphore* signalSemaphores, uint32_t signalSemaphoreCount, VkFence fence)
	{
		ValProc& proc = *computeQueue.getValProc();
		

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = cmdBufferCount;
		submitInfo.pCommandBuffers = cmdBuffers;
		submitInfo.signalSemaphoreCount = signalSemaphoreCount;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(computeQueue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
	}
}