#include <VAL/lib/system/physicalDeviceRequirements.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val
{

	std::vector<VkPhysicalDeviceType> DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(const DEVICE_TYPES flag)
	{
		std::vector<VkPhysicalDeviceType> ret;
		if (bool(flag & DEVICE_TYPES::integrated_GPU)) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		}
		if (bool(flag & DEVICE_TYPES::dedicated_GPU)) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		}
		if (bool(flag & DEVICE_TYPES::virtual_GPU)) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
		}
		if (bool(flag & DEVICE_TYPES::CPU)) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_CPU);
		}
		return ret;
	}

}