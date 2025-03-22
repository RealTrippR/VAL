#ifndef VAL_DEVICE_REQUIREMENTS_HPP
#define VAL_DEVICE_REQUIREMENTS_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {

	typedef uint8_t DEVICE_TYPE_FLAGS;
	
	enum DEVICE_TYPE_BITS
	{
		integrated_GPU = 0,
		dedicated_GPU = 1 << 1,
		virtutal_GPU = 1 << 2,
		CPU = 1 << 3
	};

	constexpr std::vector<VkPhysicalDeviceType> DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(const DEVICE_TYPE_FLAGS flag)
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
	
	typedef uint8_t DEVICE_FEATURE_FLAGS;
	enum DEVICE_FEATURE_BITS
	{
		UNDEFINED = 0,
		raytracing = 1 << 1,
		anisotropicFiltering = 1 << 2,
		variableRateShading = 1 << 3,
		geometryShader = 1 << 4,
		tesselationShader = 1 << 5,
		cubeMaps = 1 << 6
	};

	struct physicalDevicePriorities
	{
		float bandwidthCapability = 1.f;
		float memoryCapability = 1.f;
		float computeCapability = 1.f;
		float shaderInputCapability = 1.f;
	};

	struct physicalDeviceRequirements
	{
		DEVICE_TYPE_FLAGS deviceTypes = integrated_GPU | dedicated_GPU;
		DEVICE_FEATURE_FLAGS deviceFeatures{};
		physicalDevicePriorities priorities{};
		std::optional<uint32_t> deviceID;
		std::optional<uint32_t> vendorID;
		std::vector<const char*>& deviceExtensions;
		bool swapChainSupport = true;
	};
}


#endif // !VAL_DEVICE_REQUIREMENTS_HPP