#ifndef VAL_QUEUE_MANAGER_INLINE
#define VAL_QUEUE_MANAGER_INLINE

#include <stdint.h>

#include <VAL/lib/system/queueManager.hpp>

namespace val {
	inline void QueueManager::submit(const uint32_t frameidx, const VkCommandBuffer& cmdBuff, VkFence& fence, QueueManager& waitFor) 
	{
		// this needs to be changed to something optimal 
		// https://docs.vulkan.org/samples/latest/samples/performance/wait_idle/README.html
		// https://docs.vulkan.org/spec/latest/chapters/synchronization.html
		static VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitFor.getSemaphore((frameidx));
		submitInfo.pWaitDstStageMask = &waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuff;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &(_semaphores[frameidx]);


#ifndef NDEBUG
		if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
#else
		vkQueueSubmit(_queue, 1, &submitInfo, fence);
#endif // !NDEBUG
	}

	inline VkQueue QueueManager::getVkQueue() {
		return _queue;
	}

	inline VkQueueFlags QueueManager::getVkQueueFlags() const {
		return _queueFlags;
	}

	inline uint32_t QueueManager::getQueueFamily() const {
		return _queueFamily;
	}

	inline VkCommandBuffer& QueueManager::getCommandBuffer(const uint32_t& frameIdx) {
		return _commandBuffers[frameIdx];
	}

	inline VkSemaphore& QueueManager::getSemaphore(const uint32_t& frameIdx) {
		return _semaphores[frameIdx];
	}
}

#endif