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


#ifndef VAL_DEVICE_REQUIREMENTS_HPP
#define VAL_DEVICE_REQUIREMENTS_HPP

#include <stdint.h>
#include <vector>
#include <optional>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VAL/lib/classEnumBitOps.hpp>

namespace val {
	
	enum class DEVICE_TYPES : uint8_t
	{
		DEVICE_TYPE_UNDEFINED = 0,
		integrated_GPU = 1 << 0,
		dedicated_GPU = 1 << 1,
		virtual_GPU = 1 << 2,
		CPU = 1 << 3
	};

#ifndef DEVICE_TYPE_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	#define DEVICE_TYPE_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	DEF_ENUM_BITWISE_OPERATORS(DEVICE_TYPES);
#endif
	
	std::vector<VkPhysicalDeviceType> DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(const DEVICE_TYPES flag);
	
	enum class DEVICE_FEATURES : uint8_t
	{
		DEVICE_FEATURE_UNDEFINED = 0,
		raytracing = 1 << 0,
		anisotropicFiltering = 1 << 1,
		variableRateShading = 1 << 2,
		geometryShader = 1 << 3,
		tesselationShader = 1 << 4,
		cubeMaps = 1 << 5
	};

#ifndef DEVICE_FEATURE_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	#define DEVICE_FEATURE_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	DEF_ENUM_BITWISE_OPERATORS(DEVICE_FEATURES);
#endif


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
		physicalDeviceRequirements(DEVICE_TYPES deviceTypes_) : deviceTypes(deviceTypes_) {};
		physicalDeviceRequirements(DEVICE_TYPES deviceTypes_, DEVICE_FEATURES deviceFeatures_) : 
			deviceTypes(deviceTypes_), deviceFeatures(deviceFeatures_) {};
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		inline void addExtension(const char* ext) { deviceExtensions.push_back(ext); }
		inline void addFeature(DEVICE_FEATURES feature) { deviceFeatures |= feature; }
		inline void removeFeature(DEVICE_FEATURES feature) { deviceFeatures &= (~feature);}

	public:
		DEVICE_TYPES deviceTypes = (DEVICE_TYPES::integrated_GPU | DEVICE_TYPES::dedicated_GPU);
		DEVICE_FEATURES deviceFeatures{};
		physicalDevicePriorities priorities{};
		std::optional<uint32_t> deviceID;
		std::optional<uint32_t> vendorID;
		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}


#endif // !VAL_DEVICE_REQUIREMENTS_HPP