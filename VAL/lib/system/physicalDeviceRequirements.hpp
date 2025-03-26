#ifndef VAL_DEVICE_REQUIREMENTS_HPP
#define VAL_DEVICE_REQUIREMENTS_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	
	enum DEVICE_TYPE_FLAGS
	{
		DEVICE_TYPE_UNDEFINED = 0,
		integrated_GPU = 1 << 0,
		dedicated_GPU = 1 << 1,
		virtutal_GPU = 1 << 2,
		CPU = 1 << 3
	};
	DEF_ENUM_BITWISE_OPERATORS(DEVICE_TYPE_FLAGS)

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
	
	enum DEVICE_FEATURE_FLAGS
	{
		DEVICE_FEATURE_UNDEFINED = 0,
		raytracing = 1 << 0,
		anisotropicFiltering = 1 << 1,
		variableRateShading = 1 << 2,
		geometryShader = 1 << 3,
		tesselationShader = 1 << 4,
		cubeMaps = 1 << 5
	};

	/************************************************************************************/
	inline DEVICE_FEATURE_FLAGS operator|(DEVICE_FEATURE_FLAGS a, DEVICE_FEATURE_FLAGS b)
	{return static_cast<DEVICE_FEATURE_FLAGS>(static_cast<int16_t>(a) | static_cast<int16_t>(b));}
	/************************************************************************************/
	inline DEVICE_FEATURE_FLAGS operator&(DEVICE_FEATURE_FLAGS a, DEVICE_FEATURE_FLAGS b)
	{return static_cast<DEVICE_FEATURE_FLAGS>(static_cast<int16_t>(a) & static_cast<int16_t>(b));}
	/************************************************************************************/
	inline DEVICE_FEATURE_FLAGS operator^(DEVICE_FEATURE_FLAGS a, DEVICE_FEATURE_FLAGS b)
	{return static_cast<DEVICE_FEATURE_FLAGS>(static_cast<int16_t>(a) ^ static_cast<int16_t>(b));}
	/************************************************************************************/
	inline DEVICE_TYPE_FLAGS operator<<(DEVICE_FEATURE_FLAGS a, DEVICE_FEATURE_FLAGS b)
	{return static_cast<DEVICE_TYPE_FLAGS>(static_cast<int16_t>(a) << static_cast<int16_t>(b));}
	/************************************************************************************/
	inline DEVICE_TYPE_FLAGS operator>>(DEVICE_TYPE_FLAGS a, DEVICE_TYPE_FLAGS b)
	{return static_cast<DEVICE_TYPE_FLAGS>(static_cast<int16_t>(a) >> static_cast<int16_t>(b));}
	/************************************************************************************/

	struct physicalDevicePriorities
	{
		float bandwidthCapability = 1.f;
		float memoryCapability = 1.f;
		float computeCapability = 1.f;
		float shaderInputCapability = 1.f;
	};

	struct physicalDeviceRequirements
	{
		physicalDeviceRequirements() = default;
		physicalDeviceRequirements(DEVICE_TYPE_FLAGS deviceTypes_) : deviceTypes(deviceTypes_) {};
		physicalDeviceRequirements(DEVICE_TYPE_FLAGS deviceTypes_, DEVICE_FEATURE_FLAGS deviceFeatures_) : 
			deviceTypes(deviceTypes_), deviceFeatures(deviceFeatures_) {};
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		DEVICE_TYPE_FLAGS deviceTypes = integrated_GPU | dedicated_GPU;
		DEVICE_FEATURE_FLAGS deviceFeatures{};
		physicalDevicePriorities priorities{};
		std::optional<uint32_t> deviceID;
		std::optional<uint32_t> vendorID;
		std::vector<const char*> deviceExtensions;
	};
}


#endif // !VAL_DEVICE_REQUIREMENTS_HPP