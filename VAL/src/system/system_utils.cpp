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
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <ExternalLibraries/stb_image.h>;

namespace val {

#ifndef NDEBUG
	void __VAL_DEBUG_ValidateBufferCopy(uint64_t dstBufferSize, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
		if (srcOffset > dstBufferSize) {
			printf("VAL: ERROR: Cannot write out of bounds to a VkBuffer. srcOffset is greater than the size of the buffer.\n Src offset: %llu, buffer size: %llu", srcOffset, dstBufferSize);
			throw std::logic_error("VAL: ERROR: Cannot write out of bounds to a VkBuffer.srcOffset is greater than the size of the buffer.");
		}
		if (dataSize + dstOffset > dstBufferSize) {
			printf("VAL: WARNING: the size argument src val::buffer cannot be greater than the size of the dst val::buffer.\n Size of val::buffer: %u, size of source data: %u", dstBufferSize, dataSize);
		}
		else if (dataSize != dstBufferSize) {
			printf("VAL: WARNING: the size argument src val::buffer be greater than the size of the dst val::buffer.\n Size of val::buffer: %u, size of source data: %u", dstBufferSize, dataSize);
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "------------------------------------------------------\n"
			"Validation layers : " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != NULL) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != NULL) {
			func(instance, messenger, pAllocator);
		}
		else {
			printf("VAL: Failed to destroy debug messenger!\n");
		}
	}
#endif // !NDEBUG

	queueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
	{
		queueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			// Check for graphics queue support
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			// Check for presentation queue support only if surface is provided
			if (surface != NULL) {
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

				if (presentSupport) {
					indices.presentFamily = i;
				}
			}

			// Exit early if all required queues are found
			if (surface != NULL) {
				if (indices.graphicsFamily.has_value() && indices.presentFamily.has_value()) {
					break;
				}
			}
			else
			{
				if (indices.graphicsFamily.has_value()) {
					break;
				}
			}

			i++;
		}
		return indices;
	}

	std::vector<const char*> getRequiredExtensions(const bool& enableValidationLayers)
	{
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool checkValidationLayerSupport(std::vector<const char*>& validationLayers)
	{
		unsigned int layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}
		return true;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		swapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkFormat findSupportedImageFormat(VAL_PROC& proc, imageFormatRequirements& requirements) {
		return findSupportedImageFormat(proc._physicalDevice, requirements);
	}

	VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, imageFormatRequirements& requirements) {
#ifndef NDEBUG
		if (!physicalDevice) {
			printf("VAL: Could not find a supported image format, because the devices have not yet been initialized!\n");
			throw std::runtime_error("VAL: Could not find a supported image format, because the devices have not yet been initialized!");
		}
#endif // !NDEBUG

		for (VkFormat format : requirements.acceptedFormats) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (requirements.tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & requirements.features) == requirements.features) {
				return format;
			}
			else if (requirements.tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & requirements.features) == requirements.features) {
				return format;
			}
		}
		throw std::runtime_error("FAILED TO FIND A SUPPORTED IMAGE FORMAT");
	}

	VkPhysicalDevice findOptimalPhysicalDevice(VAL_PROC& proc, physicalDeviceRequirements& requirements, VkSurfaceKHR surface)
	{
		return findOptimalPhysicalDevice(proc._instance, requirements, surface);
	}

	VkPhysicalDevice findOptimalPhysicalDevice(VkInstance vkInstance, physicalDeviceRequirements& requirements, VkSurfaceKHR surface)
	{
		VkPhysicalDevice returnValue = VK_NULL_HANDLE;
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
		uint32_t deviceCount = 0u;
		physicalDevicePriorities maxDeviceRating{};
		uint16_t maxBandwidthRatingIDX = 0; /*Device index of the one with the highest bandwidth rating*/
		uint16_t maxMemoryRatingIDX = 0; /*Device index of the one with the highest mem rating*/
		uint16_t maxComputeRatingIDX = 0; /*Device index of the one with the highest compute rating*/
		uint16_t maxShaderInputRatingIDX = 0; /*Device index of the one with the highest shader input rating*/
		std::set<uint16_t> bestDevicesIndices;
		std::unordered_map<uint16_t, float> bestDevicesRatings;
		float bestRating = 0u;
		//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

		if (vkEnumeratePhysicalDevices(vkInstance, &deviceCount, VK_NULL_HANDLE) != VK_SUCCESS) {
			printf("VAL: ERROR: Failed to enumerate VkPhysicalDevices!\n");
			throw std::runtime_error("VAL: ERROR: Failed to enumerate VkPhysicalDevices!");
		}
		VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, physicalDevices);

		// all eligible devices along with their compatability weights.
		std::vector<std::pair<VkPhysicalDevice, physicalDevicePriorities /*CALCULATED PRIORITY RATINGS*/>> eligibleDevices;
		for (uint32_t i = 0; i < deviceCount; ++i) {
			VkPhysicalDevice device = physicalDevices[i];
			VkPhysicalDeviceProperties deviceProperties{};
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
			// compare against deviceRequirements
			std::vector<VkPhysicalDeviceType> allowedDeviceTypes = DEVICE_TYPE_FLAGS_TO_VkPhysicalDeviceType(requirements.deviceTypes);
			if (std::find(allowedDeviceTypes.begin(), allowedDeviceTypes.end(), deviceProperties.deviceType) == allowedDeviceTypes.end()) {
				continue; // Skip device
			}
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
			// ensure that the device has the required features
			VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures{};
			rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
			VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
			accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
			rayTracingFeatures.pNext = &accelerationStructureFeatures;
			VkPhysicalDeviceFeatures2 deviceFeatures{};
			deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			deviceFeatures.pNext = &rayTracingFeatures;
			vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
				// check required device features
			{
				DEVICE_FEATURES requiredFeatures = requirements.deviceFeatures;
				if (bool(requiredFeatures & DEVICE_FEATURES::raytracing)) {
					bool meetsRequirements =
						rayTracingFeatures.rayTracingPipeline &&
						accelerationStructureFeatures.accelerationStructure;			
					if (!meetsRequirements) {
						continue; // Skip device
					}
				}
				if (bool(requiredFeatures & DEVICE_FEATURES::anisotropicFiltering)) {
					bool meetsRequirements = 
						deviceFeatures.features.samplerAnisotropy;
					if (!meetsRequirements) {
						continue; // Skip device
					}
				}
				if (bool(requiredFeatures & DEVICE_FEATURES::variableRateShading)) {
					bool meetsRequirements = 
						deviceFeatures.features.sampleRateShading;
					if (!meetsRequirements) {
						continue; // Skip device
					}
				}
				if (bool(requiredFeatures & DEVICE_FEATURES::geometryShader)) {
					bool meetsRequirements =
						deviceFeatures.features.geometryShader;
					if (!meetsRequirements) {
						continue; // Skip device
					}
				}
				if (bool(requiredFeatures & DEVICE_FEATURES::geometryShader)) {
					bool meetsRequirements =
						deviceFeatures.features.tessellationShader;
					if (!meetsRequirements) {
						continue; // Skip device
					}
				}
			}
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
			// Check extension support
			if (checkDeviceExtensionSupport(device, requirements.deviceExtensions) == false) {
				continue; // Skip device
			}

			// Check swapchain support
			if (surface) {
				queueFamilyIndices indices = findQueueFamilies(device, surface);
				bool swapChainAdequate = false;
				swapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
				swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

				VkPhysicalDeviceFeatures supportedFeatures;
				vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

				if (false == indices.graphicsFamily.has_value() && (swapChainAdequate && indices.presentFamily.has_value()))
				{
					continue; // Skip device, it doesn't support swapchains
				}
			}
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
				// All tests passed, device is eligible
			eligibleDevices.push_back({ device, {} });
		}

		// failed to find a suitable physical device
		if (eligibleDevices.empty()) {
			goto ret;
		}
		// there is only 1 eligible device, skip unnessecary calculations and return it
		if (eligibleDevices.size() == 1) {
			returnValue = eligibleDevices[0].first;
			goto ret;
		}


		for (uint16_t i = 0; i < eligibleDevices.size(); ++i) {
			VkPhysicalDevice device = eligibleDevices[i].first;
			VkPhysicalDeviceProperties deviceProperties{};
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
			const physicalDevicePriorities& priorities = requirements.priorities;
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
			const VkPhysicalDeviceLimits& limits = deviceProperties.limits;
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

			float memoryRating = 0.f;
			for (uint32_t j = 0; j < memProperties.memoryHeapCount; ++j) {
				if (memProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
					memoryRating += memProperties.memoryHeaps[j].size;
				}
			}

			float computeRating = limits.maxComputeSharedMemorySize + limits.maxComputeWorkGroupInvocations;
			for (uint8_t j = 0; j < 3; ++j)
			{
				computeRating += (limits.maxComputeWorkGroupCount[j] * limits.maxComputeWorkGroupSize[j]);
			}
			//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

			float bandwithRating = 0.f;
			bandwithRating += limits.maxMemoryAllocationCount * 0.1f;
			bandwithRating += limits.maxImageDimension2D * 0.002f;
			bandwithRating += limits.maxBoundDescriptorSets;
			bandwithRating += limits.maxPerStageResources;

			float shaderInputRating = 0.f;
			shaderInputRating += limits.maxPushConstantsSize;
			shaderInputRating += limits.maxUniformBufferRange * .01;
			shaderInputRating += limits.maxStorageBufferRange * .001f;
			shaderInputRating += limits.maxVertexInputAttributes;
			shaderInputRating += limits.maxVertexInputAttributes;
			shaderInputRating += limits.maxVertexInputBindingStride;

			auto& devRating = eligibleDevices[i].second;
			devRating.memoryCapability = memoryRating;
			devRating.bandwidthCapability = bandwithRating;
			devRating.shaderInputCapability = shaderInputRating;
			devRating.computeCapability = computeRating;
		}

		for (uint16_t i = 0; i < eligibleDevices.size(); ++i) {
			const auto& deviceRating = eligibleDevices[i].second;

			if (deviceRating.bandwidthCapability > maxDeviceRating.bandwidthCapability) {
				maxDeviceRating.bandwidthCapability = deviceRating.bandwidthCapability;
				maxBandwidthRatingIDX = i;
			}

			if (deviceRating.memoryCapability > maxDeviceRating.memoryCapability) {
				maxDeviceRating.memoryCapability = deviceRating.memoryCapability;
				maxMemoryRatingIDX = i;
			}

			if (deviceRating.computeCapability > maxDeviceRating.computeCapability) {
				maxDeviceRating.computeCapability = deviceRating.computeCapability;
				maxComputeRatingIDX = i;
			}

			if (deviceRating.shaderInputCapability > maxDeviceRating.shaderInputCapability) {
				maxDeviceRating.shaderInputCapability = deviceRating.shaderInputCapability;
				maxShaderInputRatingIDX = i;
			}
		}

		bestDevicesIndices = { maxBandwidthRatingIDX, maxMemoryRatingIDX, maxComputeRatingIDX, maxShaderInputRatingIDX };
		for (const auto& deviceIdx : bestDevicesIndices) {
			if (deviceIdx == maxBandwidthRatingIDX) {
				bestDevicesRatings[deviceIdx] += requirements.priorities.bandwidthCapability;
			}
			if (deviceIdx == maxMemoryRatingIDX) {
				bestDevicesRatings[deviceIdx] += requirements.priorities.memoryCapability;
			}
			if (deviceIdx == maxComputeRatingIDX) {
				bestDevicesRatings[deviceIdx] += requirements.priorities.computeCapability;
			}
			if (deviceIdx == maxShaderInputRatingIDX) {
				bestDevicesRatings[deviceIdx] += requirements.priorities.shaderInputCapability;
			}
		}

		// find best device
		for (const auto& it : bestDevicesRatings) {
			if (it.second > bestRating) {
				returnValue = eligibleDevices[it.first].first;
			}
		}

#ifndef NDEBUG
		if (!returnValue) {
			printf("VAL: ERROR: Failed to find a physical device from the given device requirements!\n");
			throw std::runtime_error("VAL: ERROR: Failed to find a physical device from the given device requirements!");
;		}
#endif
		ret:
			free(physicalDevices);
		return returnValue;
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(GLFWwindow* windowHDL, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(windowHDL, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VkImageView createImageView(VkDevice device, VkImage image, const VkFormat& format, const uint32_t& mipLevels /*DEFAULT=1U*/) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("FML: FAILED TO CREATE TEXTURE VIEW");
		}

		return imageView;
	}

	VkImage createTextureImage(VAL_PROC* proc, fs::path imgFilepath, stbi_uc** pixelsOut, VkFormat format,
		VkDeviceMemory& textureImageMemory, const VkImageUsageFlagBits& additionalUsageFlagBits, const uint32_t& mipLevels,
		int* texWidthOut /*NULL BY DEFAULT*/, int* texHeightOut /*NULL BY DEFAULT*/, uint8_t* texChannelsOut /*NULL BY DEFAULT*/, const bufferSpace& buffSpace /*GPU_ONLY BY DEFAULT*/) {
		VkImage textureImage = NULL;
		int texWidth, texHeight, texChannels;

		const int imgRGBA_Type = STBI_rgb_alpha;
		*pixelsOut = stbi_load(imgFilepath.string().c_str(), &texWidth, &texHeight, &texChannels, imgRGBA_Type);
		VkDeviceSize imageSize = texWidth * texHeight * (imgRGBA_Type);

		if (!*pixelsOut) {
			dbg::printError("FAILED TO LOAD TEXTURE IMAGE, IMAGE AT FILEPATH: %ws IS OF AN UNSUPPORTED TYPE OR DOES NOT EXIST!\n", imgFilepath.c_str());
			dbg::printError("Error loading image: %s\n", stbi_failure_reason());
			throw std::runtime_error("VAL: ERROR: FAILED TO LOAD TEXTURE IMAGE");
		}
#ifndef NDEBUG
#ifndef VAL_DISABLE_IMG_PRINT_DEBUG
		else {
			dbg::printNote("Successfully loaded image %s. Image dimensions: %d x %d, Channels: %d\n", imgFilepath.string().c_str(), texWidth, texHeight, texChannels);
		}
#endif // !1
#endif // !NDEBUG


		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		// create staging buffer
		proc->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


		void* data;
		vkMapMemory(proc->_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, *pixelsOut, static_cast<size_t>(imageSize));
		vkUnmapMemory(proc->_device, stagingBufferMemory);

		proc->createImage(texWidth, texHeight, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT | additionalUsageFlagBits, VkMemoryPropertyFlags(buffSpace), textureImage, textureImageMemory, mipLevels);

		// copy staging buffer to the main image
		proc->transitionImageLayout(textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, NULL, mipLevels);
		proc->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		vkDestroyBuffer(proc->_device, stagingBuffer, nullptr);
		vkFreeMemory(proc->_device, stagingBufferMemory, nullptr);

#ifndef NDEBUG
		if ((texWidthOut || texHeightOut) && !(texWidthOut && texHeightOut)) {
			dbg::printError("VAL: If either texWidthOut or texHeightOut is a valid pointer, both must be valid, but only 1 was passed as an input to createTextureImage");
			throw std::logic_error("VAL: If either texWidthOut or texHeightOut is a valid pointer, both must be valid, but only 1 was passed as an input to createTextureImage.");
		}
#endif // !NDEBUG

		if (texWidthOut) {
			*texWidthOut = texWidth;
			*texHeightOut = texHeight;
		}
		if (texChannelsOut) {
			*texChannelsOut = texChannels;
		}
		return textureImage;
	}

	bool readByteFile(const std::string& filename, std::vector<char>* dst)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		size_t fileSize = (size_t)file.tellg();
		dst->resize(fileSize);
		file.seekg(0);
		file.read(dst->data(), fileSize);
		file.close();
		return true;
	}

	bool readByteFile(const std::string& filename, tiny_vector<char>* dst) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		size_t fileSize = (size_t)file.tellg();
		dst->resize(fileSize);
		file.seekg(0);
		file.read(dst->data(), fileSize);
		file.close();
		return true;
	}

	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("FAILED TO FIND SUITABLE MEMORY TYPE!");
	}

	VkMemoryPropertyFlags bufferSpaceToVkMemoryProperty(const bufferSpace& bufferSpace) {
		switch (bufferSpace) {
		case GPU_ONLY:
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		case CPU_GPU:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			// this can be optimized, some GPUs support VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			// it would be smart to use this memory property whenever supported.
		}
		return NULL; // fallback
	}
}