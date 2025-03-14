#include <VAL/lib/system/computeTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void computeTarget::compute(VAL_PROC& proc, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ)
	{
		const auto& currentFrame = proc._currentFrame;
		VkCommandBuffer& cmdBuffer = proc._computeQueue._commandBuffers[currentFrame];

		vkCmdDispatch(proc._computeQueue._commandBuffers[currentFrame], groupCountX, groupCountY, groupCountZ);
	}

	void computeTarget::update(VAL_PROC& proc, computePipelineCreateInfo& computePipeline)
	{
		auto& queue = proc._computeQueue;
		const auto& currentFrame = proc._currentFrame;
		VkCommandBuffer& cmdBuffer = queue._commandBuffers[currentFrame];

		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, proc._computePipelines[computePipeline.pipelineIdx]);
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, proc._computePipelineLayouts[computePipeline.pipelineIdx],
			0, 1, &proc._descriptorSets[computePipeline.descriptorsIdx][currentFrame], 0, nullptr);
	}

	void computeTarget::begin(VAL_PROC& proc)
	{
		auto& queue = proc._computeQueue;
		const auto& currentFrame = proc._currentFrame;

		// wait for the previosly submitted compute command buffer to finish
		vkWaitForFences(proc._device, 1, &queue._fences[proc._currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(proc._device, 1, &queue._fences[proc._currentFrame]);
		vkResetCommandBuffer(queue._commandBuffers[proc._currentFrame], /*VkCommandBufferResetFlagBits*/ 0);


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(queue._commandBuffers[proc._currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void computeTarget::submit(VAL_PROC& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence /*DEFAULT=VK_NULL_HANDLE*/)
	{
		if (vkEndCommandBuffer(proc._computeQueue._commandBuffers[proc._currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}

		auto& currentFrame = proc._currentFrame;
		auto& queue = proc._computeQueue;
		
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
		submitInfo.pCommandBuffers = &queue._commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &queue._semaphores[currentFrame];

		if (vkQueueSubmit(queue._queue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
	}
}