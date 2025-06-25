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
	inline void Queue::reset()
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		vkResetCommandBuffer(_commandBuffers[_proc->getCurrentFrame()], 0x0);
	}
	
	inline void Queue::begin()
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		const VkCommandBufferBeginInfo beginInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0x0,
			.pInheritanceInfo = VK_NULL_HANDLE
		};

		vkBeginCommandBuffer(getCommandBuffer(), &beginInfo);
	}


	inline void Queue::end() 
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG

		vkEndCommandBuffer(getCommandBuffer());
	}


	inline void Queue::submit(Queue& waitUpon, const VkPipelineStageFlags& waitStages)
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		VkSubmitInfo submitInfo;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = &_commandBuffers[_proc->getFramesInFlight()];
		submitInfo.commandBufferCount = 1u;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semaphores[_proc->getCurrentFrame()];
		submitInfo.pWaitDstStageMask = &waitStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitUpon._semaphores[_proc->getCurrentFrame()];

		vkQueueSubmit(_vkQueue, 1, &submitInfo, NULL);
	}

	inline void Queue::submit(Queue& waitUpon, const VkPipelineStageFlags& waitStages, const VkFence& fence)
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		VkSubmitInfo submitInfo; 
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = &(getCommandBuffer());
		submitInfo.commandBufferCount = 1u;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semaphores[_proc->getCurrentFrame()];
		submitInfo.pWaitDstStageMask = &waitStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitUpon._semaphores[_proc->getCurrentFrame()];

		vkQueueSubmit(_vkQueue, 1, &submitInfo, fence);
	}

	inline void Queue::submit(const VkSemaphore* waitSemaphores, const  uint8_t waitSemaphoreCount,
		const VkShaderStageFlags* waitStages /*1 for every semaphore*/)
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		VkSubmitInfo submitInfo;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = &_commandBuffers[_proc->getFramesInFlight()];
		submitInfo.commandBufferCount = 1u;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semaphores[_proc->getCurrentFrame()];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;

		vkQueueSubmit(_vkQueue, 1, &submitInfo, NULL);
	}

	inline void Queue::submit(const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount,
		const VkShaderStageFlags* waitStages /*1 for every semaphore*/, const VkFence& fence)
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		VkSubmitInfo submitInfo;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.pCommandBuffers = &_commandBuffers[_proc->getFramesInFlight()];
		submitInfo.commandBufferCount = 1u;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_semaphores[_proc->getCurrentFrame()];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = waitSemaphores;

		vkQueueSubmit(_vkQueue, 1, &submitInfo, fence);
	}


	inline uint8_t Queue::getCommandBufferCount() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _proc->getFramesInFlight();
	}

	inline uint8_t Queue::getSemaphoreCount() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _proc->getFramesInFlight();
	}

	inline VkCommandBuffer& Queue::getCommandBuffer() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _commandBuffers[_proc->getCurrentFrame()];
	}

	inline VkCommandBuffer& Queue::getCommandBuffer(const uint8_t frameIdx) const
	{		
		return _commandBuffers[frameIdx];
	}

	inline VkSemaphore& Queue::getSemaphore() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG

		return _semaphores[_proc->getCurrentFrame()];
	}

	inline VkSemaphore& Queue::getSemaphore(const uint8_t frameIdx) const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _semaphores[frameIdx];
	}

	inline QUEUE_FLAGS Queue::getQueueFlags() const
	{
#ifndef NDEBUG
		dbgValidateSelfUse();
#endif // !NDEBUG
		return _queueFlags;
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
}

#endif // !VAL_QUEUE_INLINE

#endif // VAL_QUEUE_HPP