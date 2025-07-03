#include <VAL/lib/system/computeTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void computeTarget::compute(Queue& computeQueue, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ)
	{
		vkCmdDispatch(computeQueue.getCommandBuffer(), groupCountX, groupCountY, groupCountZ);
	}

	void computeTarget::update(Queue& computeQueue, computePipelineCreateInfo& computePipeline)
	{
		ValProc& proc = *computeQueue.getValProc();
		const auto& currentFrame = proc.getCurrentFrame();
		VkCommandBuffer& cmdBuffer = computeQueue.getCommandBuffer();

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

	void computeTarget::submit(Queue& computeQueue, std::vector<VkSemaphore> waitSemaphores, VkFence fence /*DEFAULT=VK_NULL_HANDLE*/)
	{
		if (vkEndCommandBuffer(computeQueue.getCommandBuffer()) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}

		ValProc& proc = *computeQueue.getValProc();
		
		// SUBMIT
		VkPipelineStageFlags* waitStages = (VkPipelineStageFlags*)calloc(waitSemaphores.size(), sizeof(VkPipelineStageFlags));
		for (size_t i = 0; i < waitSemaphores.size(); ++i) {
			waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &computeQueue.getCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &computeQueue.getSemaphore();

		if (vkQueueSubmit(computeQueue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
	}
}