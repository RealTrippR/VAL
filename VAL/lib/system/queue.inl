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

#ifdef VAL_QUEUE_HPP

#ifndef VAL_QUEUE_INLINE
#define VAL_QUEUE_INLINE

#include <VAL/lib/system/queue.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val 
{
	inline void Queue::reset(VkCommandBuffer* commandBuffers, uint8_t commandBufferCount)
	{
		for (uint8_t i = 0; i < commandBufferCount; ++i)
		{
			vkResetCommandBuffer(commandBuffers[i], 0x0);
		}
	}
	
	inline void Queue::begin(VkCommandBuffer* commandBuffers, uint8_t commandBufferCount)
	{
		const VkCommandBufferBeginInfo beginInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0x0,
			.pInheritanceInfo = VK_NULL_HANDLE
		};
		for (uint8_t i = 0; i < commandBufferCount; ++i)
		{
			vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
		}
	}


	inline void Queue::end(VkCommandBuffer* commandBuffers, uint8_t commandBufferCount)
	{
		for (uint8_t i = 0; i < commandBufferCount; ++i)
		{
			vkEndCommandBuffer(commandBuffers[i]);
		}
	}


	inline void Queue::submit(
		const VkCommandBuffer* commandBuffers, uint8_t commandBufferCount,
		const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount,
		const PIPELINE_STAGE* waitStages /*1 for every semaphore*/)
	{
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.commandBufferCount = commandBufferCount;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = NULL;
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStages;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;
		vkQueueSubmit(_vkQueue, 1, &submitInfo, NULL);
	}


	inline void Queue::submit(
		const VkCommandBuffer cmdBuffer,
		const VkSemaphore waitSemaphore,
		const VkSemaphore signalSemaphore,
		const PIPELINE_STAGE* waitStages, /*1 for every wait semaphore*/
		VkFence fence
	)
	{
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = &cmdBuffer;
		submitInfo.commandBufferCount = 1;
		submitInfo.signalSemaphoreCount = (signalSemaphore) ? 1 : 0;
		submitInfo.pSignalSemaphores = &signalSemaphore;
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStages;
		submitInfo.waitSemaphoreCount = (waitSemaphore) ? 1 : 0;
		submitInfo.pWaitSemaphores = &waitSemaphore;
		vkQueueSubmit(_vkQueue, 1, &submitInfo, fence);
	}

	inline void Queue::submit(
		const VkCommandBuffer* commandBuffers, uint8_t commandBufferCount,
		const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount,
		const PIPELINE_STAGE* waitStages /*1 for every wait semaphore*/, VkFence fence)
	{
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.commandBufferCount = commandBufferCount;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = NULL;
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStages;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;
		vkQueueSubmit(_vkQueue, 1, &submitInfo, fence);
	}


	inline void Queue::submit(
		const VkCommandBuffer* commandBuffers, uint8_t commandBufferCount,
		const VkSemaphore* signalSemaphores, const uint8_t signalSemaphoreCount,
		const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount,
		const PIPELINE_STAGE* waitStages /*1 for every semaphore*/,
		VkFence fence)
	{
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.commandBufferCount = commandBufferCount;
		submitInfo.signalSemaphoreCount = signalSemaphoreCount;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStages;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;
		vkQueueSubmit(_vkQueue, 1, &submitInfo, fence);
	}

	inline uint8_t Queue::getQueueFamily() const
	{
		return _queueFamily;
	}

	inline VkQueue Queue::getVkQueue() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _vkQueue;
	}

	inline ValProc* Queue::getValProc() const
	{
		return _proc;
	}

	inline VAL_RETURN_CODE Queue::waitIdle() const
	{
		return VAL_RETURN_CODE(vkQueueWaitIdle(_vkQueue) == VK_SUCCESS);
	}

	inline QUEUE_FLAGS Queue::getQueueFlags() const
	{
		return _queueFlags;
	}
}

#endif // !VAL_QUEUE_INLINE

#endif // VAL_QUEUE_HPP