#include <VAL/lib/system/queueManager.hpp>

namespace val
{
	uint32_t queueManager::findQueueFamilyFromQueueFlags(VkPhysicalDevice physicalDevice) {

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags == _queueFlags) {
				_queueFamily = i;
				return;
			}
		}

		return _queueFamily;
	}



	void create(const uint32_t queueFamily, const uint16_t framesInFlight, bool semaphoresNeeded, bool fencesNeeded) {

	}
}