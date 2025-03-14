#include <VAL/lib/system/queueManager.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	uint32_t queueManager::findQueueFamilyFromQueueFlags(VkPhysicalDevice physicalDevice, bool isPresentQueue, VkSurfaceKHR surface) {

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (isPresentQueue) {
				VkBool32 presentSupport = false;
				_queueFamily = i;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
#ifndef NDEBUG
				if (!presentSupport) {
					printf("VAL: PRESENT QUEUE REQUESTED, BUT IS NOT AVAILABLE ON THIS DEVICE!\n");
				}
#endif // !NDEBUG
				return _queueFamily;
			}

			if (queueFamily.queueFlags & _queueFlags) {
				_queueFamily = i;
				return _queueFamily;
			}
		}

		return _queueFamily;
	}



	void queueManager::create(VAL_PROC& proc, bool semaphoresNeeded, bool fencesNeeded) {
		_commandBuffers.resize(proc._MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = proc._commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

		if (vkAllocateCommandBuffers(proc._device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		// clear existing semaphores to prevent double creation error
		for (VkSemaphore& semaphore : _semaphores) {
			if (semaphore != VK_NULL_HANDLE) {
				vkDestroySemaphore(proc._device, semaphore, NULL);
;			}
		}
		// clear existing fences to prevent double creation error
		for (VkFence& fence : _fences) {
			if (fence != VK_NULL_HANDLE) {
				vkDestroyFence(proc._device, fence, NULL);
			}
		}

		if (semaphoresNeeded) {
			_semaphores.resize(proc._MAX_FRAMES_IN_FLIGHT);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			for (uint32_t i = 0; i < _semaphores.size(); ++i) {
				if (vkCreateSemaphore(proc._device, &semaphoreInfo, nullptr, &_semaphores[i]) != VK_SUCCESS) {
					printf("VAL: FAILED TO CREATE SEMAPHORE #%d FOR QUEUE MANAGER: %h\n", i, this);
					throw std::runtime_error("VAL: FAILED TO CREATE  SEMAPHORE FOR QUEUE MANAGER!");
				}
			}
		}

		if (fencesNeeded) {
			_fences.resize(proc._MAX_FRAMES_IN_FLIGHT);

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (uint32_t i = 0; i < _fences.size(); ++i) {
				if (vkCreateFence(proc._device, &fenceInfo, nullptr, &_fences[i]) != VK_SUCCESS) {
					printf("VAL: FAILED TO CREATE FENCE #%d FOR QUEUE MANAGER: %h\n", i, this);
					throw std::runtime_error("VAL: FAILED TO CREATE FENCE FOR QUEUE MANAGER!");
				}
			}
		}
	}

	VkDeviceQueueCreateInfo queueManager::getQueueCreateInfo() {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = _queueFamily;
		queueCreateInfo.queueCount = 1;
		return queueCreateInfo;
	}

	void queueManager::destroy(VAL_PROC& proc) {
		if (_queue == NULL) {
			return;
		}

		vkQueueWaitIdle(_queue); // wait for semaphores to finish

		vkFreeCommandBuffers(proc._device, proc._commandPool, _commandBuffers.size(), _commandBuffers.data());

		for (int i = 0; i < _semaphores.size(); ++i) {
			vkDestroySemaphore(proc._device, _semaphores[i], VK_NULL_HANDLE);
		}

		for (int i = 0; i < _fences.size(); ++i) {
			vkDestroyFence(proc._device, _fences[i], VK_NULL_HANDLE);
		}

		_semaphores.clear();
		_commandBuffers.clear();
		_fences.clear();
		_queue = NULL;
	}
}