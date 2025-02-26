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
			0, 1, &proc._descriptorSets[computePipeline.pipelineIdx][currentFrame], 0, nullptr);


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


		if (vkBeginCommandBuffer(queue._commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording compute command buffer!");
		}
	}
}