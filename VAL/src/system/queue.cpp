#include <VAL/lib/system/queue.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <map>

namespace val
{
	// uint16: queue family, bool: supports present
	static std::map<QUEUE_FLAGS, std::pair<uint16_t, bool>> queueFlagsToFamilyIndicesCache;

	void Queue::create(ValProc& proc, const QUEUE_FLAGS flags)
	{
		destroy();
		_queueFlags = flags;
		_proc = &proc;
		create();
	}

	void Queue::create(ValProc& proc, const uint8_t cmdBuffAndSemaphoreCount, const QUEUE_FLAGS flags)
	{
		destroy();
		_queueFlags = flags;
		_proc = &proc;
		create();
	}


	void Queue::create()
	{
		
		if (_proc == NULL || _proc->getVkLogicalDevice() == NULL) 
		{
			dbg::printError("Cannot create Queue @ %p, the ValProc it's associated with is uninitialized", this);
			throw std::runtime_error("Cannot create Queue, the ValProc it's associated with is uninitialized");
		}



		uint8_t qfam = 0x0;
		// get the queue family
		if (findQueueFamily(&qfam) == false) {
#ifndef NDEBUG
			dbg::printError("Failed create Queue @ %p with flags as U32 %u because the queue family could not be found.", this, _queueFlags);
			throw std::runtime_error("Invalid queue flags, cannot create Queue.");
#endif // !NDEBUG
		}

		vkGetDeviceQueue(_proc->getVkLogicalDevice(), qfam, 0, &_vkQueue);
	}

	void Queue::destroy()
	{
		if (_vkQueue) 
		{
			// wait until queue is finished (we can't destroy semaphores that are currently in use)
			vkQueueWaitIdle(_vkQueue);
			_vkQueue = NULL;
		}
	}

	void Queue::copyToOther(Queue* other) const
	{
		other->destroy();

		other->_vkQueue = this->_vkQueue;
		other->_queueFlags = this->_queueFlags;
		other->_proc = this->_proc;
		other->_queueFamily = this->_queueFamily;
		other->create();
	}

#ifndef NDEBUG
	void Queue::dbgValidateSelfUse() const
	{
		if (_proc == NULL) {
			dbg::printWarning("Attempting to use Queue @ %p which has not been created yet will result in undefined behavior.");
		}
	}
#endif // !NDEBUG

	
	bool Queue::findQueueFamily(uint8_t* queueFamilyOut,VkSurfaceKHR surface)
	{
#ifndef NDEBUG
		dbgValidateSelfUse();

		if (uint16_t(_queueFlags) == 0x0) {
			dbg::printWarning("The queue family of Queue @ %p will not be found because it's queue flags are not set (0).", this);
		}
#endif // !NDEBUG

		bool findRes = false;
		uint8_t qfam = findQueueFamilyOfQueueFlags(_queueFlags, _proc, surface, &findRes);

		if (findRes == false) {
			dbg::printWarning("Failed to find the queue family of Queue @ %p.", this);
		}

		if (queueFamilyOut) {
			*queueFamilyOut = qfam;
		}
		return findRes;
	}

	uint8_t Queue::findQueueFamilyOfQueueFlags(QUEUE_FLAGS queueFlags, ValProc* proc, VkSurfaceKHR surface, bool* success)
	{
		uint8_t queueFamily = 0xFF;
		// first check if the queue family matching queue flags has been cached
		if (queueFlagsToFamilyIndicesCache.count(queueFlags) == 0 
			|| (surface && queueFlagsToFamilyIndicesCache[queueFlags].second == false)) /*check present support*/
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(proc->getVkPhysicalDevice(), &queueFamilyCount, nullptr);

			tiny_vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(proc->getVkPhysicalDevice(), &queueFamilyCount, queueFamiliesProperties.data());

			for (uint8_t i = 0; i < queueFamiliesProperties.size(); ++i)
			{
				VkQueueFamilyProperties& queueProperties = queueFamiliesProperties[i];

				// find present queue support
				VkBool32 presentSupport = false;

			
				if (queueProperties.queueFlags & VkQueueFlags(queueFlags))
				{
					if (surface)
					{
						// https://www.reddit.com/r/vulkan/comments/z9lqmo/docs_a_bit_unclear/
						vkGetPhysicalDeviceSurfaceSupportKHR(proc->getVkPhysicalDevice(), i, surface, &presentSupport);

						if (presentSupport == false) {
							if (success) {
								*success = false;
							}
							continue;
						}
					}


					queueFlagsToFamilyIndicesCache[queueFlags] = { i, presentSupport };
					queueFamily = i;
					if (success) {
						*success = true;
					}
					break;
				}
			}
		}
		else {
			queueFamily = (uint8_t)queueFlagsToFamilyIndicesCache[queueFlags].first;
			if (success) {
				*success = true;
			}
		}

		return queueFamily;
	}
}