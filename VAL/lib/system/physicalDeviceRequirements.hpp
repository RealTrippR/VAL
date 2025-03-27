#ifndef VAL_DEVICE_REQUIREMENTS_HPP
#define VAL_DEVICE_REQUIREMENTS_HPP

#include <stdint.h>
#include <vector>
#include <optional>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define DEF_ENUM_BITWISE_OR(TYPE) inline TYPE operator|(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND(TYPE) inline TYPE operator&(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR(TYPE) inline TYPE operator^(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_NOT(TYPE) inline TYPE operator~(TYPE a) { return static_cast<TYPE>(~static_cast<std::underlying_type_t<TYPE>>(a));}

#define DEF_ENUM_BITWISE_OR_ASSIGN(TYPE) inline TYPE operator|=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND_ASSIGN(TYPE) inline TYPE operator&=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE) inline TYPE operator^=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}

#define DEF_ENUM_BITWISE_OPERATORS(TYPE)\
DEF_ENUM_BITWISE_OR(TYPE)\
DEF_ENUM_BITWISE_AND(TYPE)\
DEF_ENUM_BITWISE_XOR(TYPE)\
DEF_ENUM_BITWISE_NOT(TYPE)\
DEF_ENUM_BITWISE_OR_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_AND_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE)


namespace val {
	
	enum DEVICE_TYPE_FLAGS
	{
		DEVICE_TYPE_UNDEFINED = 0,
		integrated_GPU = 1 << 0,
		dedicated_GPU = 1 << 1,
		virtutal_GPU = 1 << 2,
		CPU = 1 << 3
	};

	DEF_ENUM_BITWISE_OPERATORS(DEVICE_TYPE_FLAGS);
	
	std::vector<VkPhysicalDeviceType> DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(const DEVICE_TYPE_FLAGS flag);
	
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

	DEF_ENUM_BITWISE_OPERATORS(DEVICE_FEATURE_FLAGS);


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
		DEVICE_TYPE_FLAGS deviceTypes = (integrated_GPU | dedicated_GPU);
		DEVICE_FEATURE_FLAGS deviceFeatures{};
		physicalDevicePriorities priorities{};
		std::optional<uint32_t> deviceID;
		std::optional<uint32_t> vendorID;
		std::vector<const char*> deviceExtensions;
	};
}


#endif // !VAL_DEVICE_REQUIREMENTS_HPP