#include <VAL/lib/system/physicalDeviceRequirements.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val
{

	std::vector<VkPhysicalDeviceType> DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(const DEVICE_TYPE_FLAGS flag)
	{
		std::vector<VkPhysicalDeviceType> ret;
		if (flag & integrated_GPU) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
		}
		if (flag & dedicated_GPU) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		}
		if (flag & virtutal_GPU) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
		}
		if (flag & CPU) {
			ret.push_back(VK_PHYSICAL_DEVICE_TYPE_CPU);
		}
		return ret;
	}

}