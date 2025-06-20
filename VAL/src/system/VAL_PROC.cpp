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

#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {

	// the push descriptor hashmap will be destroyed after pipeline creation to optimize ram usage
	std::unordered_map<pipelineCreateInfo*, uint_fast32_t>* pushDescriptorIdxMap = NULL;
	void initPushDescriptorTrackMap() {
		pushDescriptorIdxMap = new std::unordered_map<pipelineCreateInfo*, uint_fast32_t>;
	}

	inline void setPushDescriptorIndexOfPipeline(pipelineCreateInfo* pipeline, const uint_fast32_t idx) {
		(*pushDescriptorIdxMap)[pipeline] = idx;
	}

	inline uint_fast32_t getPushDescriptorIndexOfPipeline(pipelineCreateInfo* pipeline) {
		return (*pushDescriptorIdxMap)[pipeline];
	}

	void destroyPushDescriptorTrackMap() {
		delete pushDescriptorIdxMap;
	}

	void VAL_PROC::initDevices(val::physicalDeviceRequirements& physical_Device_Requirements, const std::vector<const char*>& validationLayers,
		const bool enableValidationLayers, val::window* windowVAL)
	{
		_windowVAL = windowVAL;

		createVK_Instance(validationLayers, enableValidationLayers);
		setupDebugMessenger(enableValidationLayers);

		if (windowVAL) {
			windowVAL->createWindowSurface(_instance);
		}

		pickPhysicalDevice(physical_Device_Requirements);
		vkGetPhysicalDeviceProperties(_physicalDevice, &_physicalDeviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures{};
		if (bool(physical_Device_Requirements.deviceFeatures | DEVICE_FEATURES::anisotropicFiltering)) {
			deviceFeatures.samplerAnisotropy = VK_TRUE;
		}
		if (bool(physical_Device_Requirements.deviceFeatures | DEVICE_FEATURES::geometryShader)) {
			deviceFeatures.geometryShader = VK_TRUE;
		}
		if (bool(physical_Device_Requirements.deviceFeatures | DEVICE_FEATURES::tesselationShader)) {
			deviceFeatures.tessellationShader = VK_TRUE;
		}
		if (bool(physical_Device_Requirements.deviceFeatures | DEVICE_FEATURES::cubeMaps)) {
			deviceFeatures.imageCubeArray = VK_TRUE;
		}
		if (bool(physical_Device_Requirements.deviceFeatures | DEVICE_FEATURES::variableRateShading)) {
			deviceFeatures.variableMultisampleRate = VK_TRUE;
		}

		createLogicalDevice(physical_Device_Requirements.deviceExtensions, validationLayers, enableValidationLayers, &deviceFeatures, windowVAL);

		const auto it = std::find(physical_Device_Requirements.deviceExtensions.begin(), physical_Device_Requirements.deviceExtensions.end(), VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
		// only load the vkCmdPushDescriptorSetKHR function is the extension for it is enabled
		if (it != physical_Device_Requirements.deviceExtensions.end()) {
			pipelineCreateInfo_loadvkCmdPushDescriptorSetKHR(_device);
		}
	}

	// creates the pipelines, descriptor sets and descriptor pools, queues, and the window.
	void VAL_PROC::create(val::window* windowVAL, const uint8_t MaxFramesInFlight,
		const VkFormat swapchainFormat, std::vector<graphicsPipelineCreateInfo*> pipelineCreateInfos,
		std::vector<computePipelineCreateInfo*> computePipelineCreateInfos)
	{
		_MAX_FRAMES_IN_FLIGHT = MaxFramesInFlight;
		_windowVAL = windowVAL;

		initPushDescriptorTrackMap();
		
		std::vector<pipelineCreateInfo*> combinedCreateInfos;
		uint32_t idx = 0;
		for (graphicsPipelineCreateInfo* info : pipelineCreateInfos) {
			combinedCreateInfos.push_back((pipelineCreateInfo*)info);

			info->pipelineIdx = idx;

			idx++;
		}

		for (computePipelineCreateInfo* info : computePipelineCreateInfos) {
			combinedCreateInfos.push_back((pipelineCreateInfo*)info);

			info->pipelineIdx = idx - pipelineCreateInfos.size();

			idx++;
		}

		// set correct _id for specialization constants
		for (size_t i = 0; i < combinedCreateInfos.size(); ++i) {
			for (size_t j = 0; j < combinedCreateInfos[i]->shaders.size(); ++j) {
				shader& shdr = *(combinedCreateInfos[i]->shaders[j]);
				for (size_t k = 0; k < shdr._specializationConstants.size(); ++k) {
					auto& constant = shdr._specializationConstants[k];
					constant.values[0]->_id = constant.bindingIndex; // set id
				}
			}
		}


		// check the push descriptor ext. is present
		bool pushDescriptorsEnabled = false;
		for (uint_fast8_t i = 0; i < _deviceExtCount; ++i) {
			if (strcmp(_deviceExtensions[i], VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME) == 0) {
				pushDescriptorsEnabled = true;
				break;
			}
		}

		createCommandPool();

		if (_windowVAL) {
			_windowVAL->createSwapChain(swapchainFormat);
			_windowVAL->createSwapChainImageViews(swapchainFormat);
		}

		_descriptorSetLayouts.resize(pipelineCreateInfos.size() + computePipelineCreateInfos.size());

		_descriptorPools.resize(pipelineCreateInfos.size() + computePipelineCreateInfos.size());

		_descriptorSets.resize(pipelineCreateInfos.size() + computePipelineCreateInfos.size());

		// allocate VK push constant memory
		size_t bytesToAllocateForPushConstants = 0;
		// calculate bytes to alloc.
		for (auto& createInfo : pipelineCreateInfos) {
			for (auto& pushConstant : createInfo->getUniquePushConstants()) {
				bytesToAllocateForPushConstants += pushConstant->_size;
			}
		}



		// In Vulkan, multiple graphics pipelines can render to the same render pass
		// first we have to get all the unique render pass manager to avoid duplication errors
		std::set<renderPassManager*> uniqueRenderPassManagers;
		// creates graphics pipelines
		for (int i = 0; i < pipelineCreateInfos.size(); ++i) {
			const graphicsPipelineCreateInfo* pipelineCreateInfo = pipelineCreateInfos[i];
			uniqueRenderPassManagers.insert(pipelineCreateInfo->renderPass);
		}

		for (auto& passMngr : uniqueRenderPassManagers) {
			createRenderPass(passMngr);
		}

		// creates graphics pipelines
		for (int i = 0; i < pipelineCreateInfos.size(); ++i) {
			graphicsPipelineCreateInfo* pipelineCreateInfo = pipelineCreateInfos[i];

			createDescriptorSetLayout(pipelineCreateInfo, i);

			createDescriptorPool(pipelineCreateInfo, i);

			if (pushDescriptorsEnabled) {
				createPushDescriptorSetLayouts(pipelineCreateInfo);
			}

			// CREATE IMAGE SAMPLERS
			for (size_t i = 0; i < pipelineCreateInfo->shaders.size(); ++i) {
				pipelineCreateInfo->shaders[i]->createImageSamplers(this);
			}
		}

		createGraphicsPipelines(pipelineCreateInfos);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create compute pipelines

		for (int i = 0; i < computePipelineCreateInfos.size(); ++i) {
			computePipelineCreateInfo* pipelineCreateInfo = computePipelineCreateInfos[i];

			createDescriptorSetLayout(pipelineCreateInfo, i + _graphicsPipelines.size());

			createDescriptorPool(pipelineCreateInfo, i + _graphicsPipelines.size());

 			if (pushDescriptorsEnabled) {
				createPushDescriptorSetLayouts(pipelineCreateInfo);
			}
		}

		createComputePipelines(computePipelineCreateInfos);

		createUniformBuffers(combinedCreateInfos);
		createSSBOs(combinedCreateInfos);

		createSyncObjects();
		
		cleanupTmpDeviceExtensions();

		destroyPushDescriptorTrackMap();
	}

	void VAL_PROC::cleanup()
	{
		cleanupTmpDeviceExtensions();

		for (size_t i = 0; i < _pipelineLayouts.size(); ++i) {
			vkDestroyPipeline(_device, _graphicsPipelines[i], NULL);

			vkDestroyPipelineLayout(_device, _pipelineLayouts[i], NULL);
		}
		_pipelineLayouts.clear();
		//vkDestroyRenderPass(_device, _renderPass, NULL);

		/*vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);

		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);*/

		
		/***********************************************************************************/
		// Destroy UBOS
		_uboArr.destroy(*this);
		/*for (uint8_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; ++i) {
			if (_uniformBuffers.empty()) {
				break;
			}
			for (VkBuffer& buff : _uniformBuffers[i]) {
				vkDestroyBuffer(_device,buff,NULL);
			}

			for (VkDeviceMemory& buffMem : _uniformBuffersMemory[i]) {
				vkFreeMemory(_device,buffMem,NULL);
			}
		}
		_uniformBuffers.clear();
		_uniformBuffersMemory.clear();*/

		/***********************************************************************************/
		// Destroy SSBOs
		for (uint8_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; ++i) {
			if (_SSBO_Buffers.empty()) {
				break;
			}
			for (VkBuffer buff : _SSBO_Buffers[i]) {
				vkDestroyBuffer(_device, buff, NULL);
			}
			_SSBO_Buffers.clear();

			for (VkDeviceMemory& buffMem : _SSBO_Memory[i]) {
				vkFreeMemory(_device, buffMem, NULL);
			}
		}
		_SSBO_Buffers.clear();
		_SSBO_Memory.clear();

		for (int i = 0; i < _descriptorPools.size(); ++i) {
			vkDestroyDescriptorPool(_device, _descriptorPools[i], NULL);
			vkDestroyDescriptorSetLayout(_device, _descriptorSetLayouts[i], NULL);
		}
		_descriptorPools.clear();

		if (_windowVAL != NULL) {
			_windowVAL = NULL;
		}
		
		_graphicsQueue.destroy(*this);
		_computeQueue.destroy(*this);
		_transferQueue.destroy(*this);


		if (_commandPool) {
			vkDestroyCommandPool(_device, _commandPool, NULL);
			_commandPool = NULL;
		}

#ifndef NDEBUG
		if (_debugMessenger) {
			destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, NULL);
			_debugMessenger = NULL;
		}
#endif // !NDEBUG

		if (_device) {
			vkDestroyDevice(_device, NULL);
			vkDestroyInstance(_instance, NULL);
			_device = NULL;
			_instance = NULL;
		}
	}

	VkDevice VAL_PROC::getVkLogicalDevice() {
		return _device;
	}

	VkPhysicalDevice VAL_PROC::getVkPhysicalDevice() {
		return _physicalDevice;
	}

	const uint32_t& VAL_PROC::getCurrentFrame() {
		return _currentFrame;
	}

	////////////////////////////////////////////////////////////////////////////////////////////

	void VAL_PROC::setupDebugMessenger(const bool& enableValidationLayers)
	{
		if (!enableValidationLayers) {
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);
#ifndef NDEBUG
		if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO SET UP DEBUG MESSENGER");
		}
#endif // !NDEBUG
	}

	void VAL_PROC::pickPhysicalDevice(val::physicalDeviceRequirements& requirements)
	{

		_physicalDevice = findOptimalPhysicalDevice(_instance, requirements, _windowVAL->_surface);

		if (_physicalDevice == VK_NULL_HANDLE) {
#ifndef NDEBUG
			dbg::printError("Failed to find a physical device that meets the given device requirements.");
#endif // !NDEBUG
			throw std::runtime_error("VAL: ERROR: Failed to find a physical device that meets the given requirements!");
		}
#ifndef NDEBUG
		VkPhysicalDeviceProperties p{};
		vkGetPhysicalDeviceProperties(_physicalDevice, &p);
		dbg::printNote("Picked physical device %s from device requirements", &p.deviceName);
#endif // !NDEBUG
	}

	void VAL_PROC::createVK_Instance(std::vector<const char*> validationLayers, const bool& enableValidationLayers)
	{
		if (enableValidationLayers && !checkValidationLayerSupport(validationLayers))
		{
			dbg::printError("Validation layers requested, but are not available.");
			throw std::runtime_error("VAL: ERROR: VALIDATION LAYERS REQUESTED, BUT NOT AVAILABLE!");
		}

		VkApplicationInfo appInfo{ };
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Unnamed";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 296);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions(enableValidationLayers);
		createInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledExtensionCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("VAL: ERROR Failed to create vulkan instance!");
		}
	}

	void VAL_PROC::createLogicalDevice(std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers,
		const bool& enableValidationLayers, VkPhysicalDeviceFeatures* deviceFeatures, val::window* windowVAL)
	{
		_graphicsQueue._queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
		_computeQueue._queueFlags = VK_QUEUE_GRAPHICS_BIT;
		_transferQueue._queueFlags = VK_QUEUE_TRANSFER_BIT;

		_graphicsQueue.findQueueFamilyFromQueueFlags(_physicalDevice);
		_computeQueue.findQueueFamilyFromQueueFlags(_physicalDevice);
		_transferQueue.findQueueFamilyFromQueueFlags(_physicalDevice);

		if (windowVAL) {
			windowVAL->_presentQueue.findQueueFamilyFromQueueFlags(_physicalDevice, true, windowVAL->_surface);
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {
			_graphicsQueue._queueFamily,
			_computeQueue._queueFamily,
			_transferQueue._queueFamily
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = deviceFeatures;


		createInfo.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// this array will be discarded after creation of the VAL_PROC
		_deviceExtensions = (char**)malloc(sizeof(char*) * deviceExtensions.size());
		_deviceExtCount = deviceExtensions.size();
		for (uint8_t i = 0; i < _deviceExtCount; ++i) {
			const size_t len = strlen(deviceExtensions[i]);
			_deviceExtensions[i] = (char*)malloc(len+1);
			strcpy_s(_deviceExtensions[i], len+1, deviceExtensions[i]);
		}

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(_physicalDevice, &createInfo, NULL, &_device) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE LOGICAL DEVICE!");
		}

		// load the vkCmdPushDescriptorSetKHR function
		PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR =
			(PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(_device, "vkCmdPushDescriptorSetKHR");

		vkGetDeviceQueue(_device, _graphicsQueue._queueFamily, 0, &_graphicsQueue._queue);
		vkGetDeviceQueue(_device, _computeQueue._queueFamily, 0, &_computeQueue._queue);
		vkGetDeviceQueue(_device, _transferQueue._queueFamily, 0, &_transferQueue._queue);


		if (windowVAL) {
#ifndef NDEBUG
			if (!windowVAL->_surface) {
				throw std::logic_error("QUEUES FOR A WINDOW CANNOT BE CREATED IF THE WINDOW SURFACE IS NULL!");
			}
#endif // !NDEBUG

			//! ERROR BC THIS BLOCK OF CODE IS NOT RUNNING. CONSIDER MOVING THIS PART OF THE FUNCTION TO THE WINDOW CLASS.
			vkGetDeviceQueue(_device, windowVAL->_presentQueue._queueFamily, 0, &windowVAL->_presentQueue._queue);
		}
	}

	// creates the graphics queue, compute queue, transfer queue, and the window's present queue (if the proc has a window attached to it) along with their respective semaphores and fences.
	void VAL_PROC::createSyncObjects() {
		if (_windowVAL) {
			_windowVAL->_presentQueue.findQueueFamilyFromQueueFlags(_physicalDevice, true, _windowVAL->_surface);
			_windowVAL->_presentQueue.create(*this, true, true);
		}

		_graphicsQueue.create(*this, true, true);
		_computeQueue.create(*this, true, true);
		_transferQueue.create(*this, true, true);
	}



	void VAL_PROC::createRenderPass(val::renderPassManager* renderPassManager) {
		// first check to make sure that it hasn't already been created
		if (renderPassManager->_VKrenderPass != VK_NULL_HANDLE) {
#ifndef NDEBUG
			dbg::printNote("Creation of render pass at address %h has been skipped as it has already been created!\n", renderPassManager);
#endif // !NDEBUG
			return;
		}
		renderPassManager->update();
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassInfo.attachmentCount = renderPassCreateInfo->attachments.size();
		renderPassInfo.attachmentCount = renderPassManager->_VkAttachments.size();
		renderPassInfo.pAttachments = renderPassManager->_VkAttachments.data();
		renderPassInfo.subpassCount = renderPassManager->_VkSubpasses.size();
		renderPassInfo.pSubpasses = renderPassManager->_VkSubpasses.data();
		renderPassInfo.dependencyCount = renderPassManager->_VkSubpassDependencies.size();
		renderPassInfo.pDependencies = renderPassManager->_VkSubpassDependencies.data();

		if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &(renderPassManager->_VKrenderPass)) != VK_SUCCESS) {
			printf("VAL: FAILED TO CREATE RENDER PASS!\n");
			throw std::runtime_error("VAL: FAILED TO CREATE RENDER PASS!");
		}
	}

	void VAL_PROC::createDescriptorPool(pipelineCreateInfo* pipelineInfo, const uint32_t pipelineIdx) {
		std::vector<VkDescriptorPoolSize> poolSizes{};

		for (shader* shader : pipelineInfo->shaders) {
			{ // UBOS
				uint32_t totalUBOCount = 0; // for array binding support
				for (uint32_t i = 0; i < shader->_UBO_Handles.size(); ++i) {
					totalUBOCount += shader->_UBO_Handles[i].values.size();
				}
				if (shader->_UBO_Handles.size() > 0) {
					/*INC*/poolSizes.resize(poolSizes.size() + 1);
					const uint32_t idx = poolSizes.size() - 1;
					poolSizes[idx].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT) * shader->_UBO_Handles.size();
				}
			}
			{ // SSBOs
				uint32_t totalSSBOCount = 0; // for array binding support
				for (uint32_t i = 0; i < shader->_SSBO_Handles.size(); ++i) {
					totalSSBOCount += shader->_SSBO_Handles[i].values.size();
				}
				if (shader->_SSBO_Handles.size() > 0) {
					/*INC*/poolSizes.resize(poolSizes.size() + 1);
					const uint32_t idx = poolSizes.size() - 1;
					poolSizes[idx].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT) * shader->_SSBO_Handles.size();
				}
			}
			{ // Samplers
				std::vector<val::sampler*> combinedImageSamplers;
				std::vector<val::sampler*> standaloneImageSamplers;

				for (val::descriptorBinding<val::sampler*>& descBinding : shader->_imageSamplers) {
					for (val::sampler* sampler : descBinding.values) {
						if (sampler->getSamplerType() == combinedImage) {
							combinedImageSamplers.push_back(sampler);
						}
						else if (sampler->getSamplerType() == standalone) {
							standaloneImageSamplers.push_back(sampler);
						}
					}
				}

				if (combinedImageSamplers.size() > 0) {
					/*INC*/poolSizes.resize(poolSizes.size() + 1);
					const uint32_t idx = poolSizes.size() - 1;
					poolSizes[idx].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT * combinedImageSamplers.size());
				}
				if (standaloneImageSamplers.size() > 0) {
					/*INC*/poolSizes.resize(poolSizes.size() + 1);
					const uint32_t idx = poolSizes.size() - 1;
					poolSizes[idx].type = VK_DESCRIPTOR_TYPE_SAMPLER;
					poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT * standaloneImageSamplers.size());
				}
			}
			{  // Textures
				uint32_t totalTextureCount = 0; // for array binding support
				for (uint32_t i = 0; i < shader->_textures.size(); ++i) {
					totalTextureCount += shader->_textures[i].values.size();
				}
				if (shader->_textures.size() > 0) {
					/*INC*/poolSizes.resize(poolSizes.size() + 1);
					const uint32_t idx = poolSizes.size() - 1;
					poolSizes[idx].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					//poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT) * shader->_textures.size();
					poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT) * totalTextureCount;
				}
			}
			//if (shader->_imageSamplers.size()>0) {
			//	/*INC*/poolSizes.resize(poolSizes.size() + 1);
			//	const uint32_t idx = poolSizes.size() - 1;
			//	poolSizes[idx].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//	poolSizes[idx].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT *
			//									shader->_imageSamplers.size());
			//}
		}

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPools[pipelineIdx]) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE DESCRIPTOR POOL");
		}

#ifndef NDEBUG
		dbg::printNote("PipelineIdx: %d - Succesfully created descriptor pools\n     poolSizeCount: %d\n     maxSets: %d\n", pipelineIdx, poolInfo.poolSizeCount, poolInfo.maxSets);
#endif // !NDEBUG
	}

	uint32_t descriptorIdxCounter() {
		static uint32_t counter = 0;
		return counter++;
	}

	void VAL_PROC::createDescriptorSetLayout(pipelineCreateInfo* createInfo, const uint32_t pipelineIdx) {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

		for (shader* shdr : createInfo->shaders) {
			const std::vector<VkDescriptorSetLayoutBinding>& shrBindings = *(shdr->getLayoutBindings());
			layoutBindings.insert(layoutBindings.end(), shrBindings.begin(), shrBindings.end());
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();

		createInfo->descriptorsIdx = descriptorIdxCounter();
		if (vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayouts[pipelineIdx]) != VK_SUCCESS) {
			throw std::runtime_error("VAL: FAILED TO CREATE DESCRIPTOR SETS!");
		}

#ifndef NDEBUG
		printf("VAL: PipelineIdx: %d - Succesfully created descriptor sets\n     Binding Count: %d\n", pipelineIdx, layoutInfo.bindingCount);
#endif // !NDEBUG
	}
	void VAL_PROC::createPushDescriptorSetLayouts(pipelineCreateInfo* createInfo) {

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (shader* shdr : createInfo->shaders) {
			const std::vector<VkDescriptorSetLayoutBinding>& shrBindings = *(shdr->getPushDescriptorLayoutBindings());
			layoutBindings.insert(layoutBindings.end(), shrBindings.begin(), shrBindings.end());
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (uint32_t)layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();
		layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;

		_pushDescriptorSetLayouts.resize(_pushDescriptorSetLayouts.size() + 1);
		//createInfo->pushDescriptorsIdx = descriptorIdxCounter();
		//createInfo->pushDescriptorsIdx = _pushDescriptorSetLayouts.size() - 1;

		 // this value will be set to it's actual value later; this flag just says that it has a push descriptor set
		createInfo->pushDescriptorsSetNo = UINT32_MAX - 1;
		setPushDescriptorIndexOfPipeline(createInfo, _pushDescriptorSetLayouts.size()-1);
		if (vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_pushDescriptorSetLayouts[_pushDescriptorSetLayouts.size()-1]) != VK_SUCCESS) {
			throw std::runtime_error("VAL: FAILED TO CREATE DESCRIPTOR SETS!");
		}
	}


	/*
	In Vulkan, descriptor sets are used to manage and organize resources
	(such as textures, buffers, or samplers) that shaders need to access during execution. They play a central role in linking
	shader programs to the resources they require. Descriptor sets allow the GPU to efficiently access and bind resources, while providing
	the flexibility and control needed for high-performance graphics programming.
	*/
	// https://vkguide.dev/docs/chapter-4/descriptors/
	// Most physical devices allow for at least 4 descriptor sets
	void VAL_PROC::createDescriptorSets(pipelineCreateInfo* createInfo) {
		const auto descriptorsIdx = createInfo->descriptorsIdx;
		// Create descriptor set layouts for each frame (same layout for each frame)
		std::vector<VkDescriptorSetLayout> layouts(_MAX_FRAMES_IN_FLIGHT, _descriptorSetLayouts[descriptorsIdx]);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPools[descriptorsIdx];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		_descriptorSets[descriptorsIdx].resize(_MAX_FRAMES_IN_FLIGHT);

#ifndef NDEBUG
		// warn if the program is attempted to create already existing descriptor sets
		if (_descriptorSets[descriptorsIdx][0] != NULL) {
			printf("VAL: Warning: decriptor sets of pipeline with descriptor idx %d have already been created, attempting to recreate existing descriptor sets will result in an error!\n", descriptorsIdx);
		}
#endif // !NDEBUG

		

		if (vkAllocateDescriptorSets(_device, &allocInfo, _descriptorSets[descriptorsIdx].data()) != VK_SUCCESS) {
			printf("VAL: FAILED TO ALLOCATE DESCRIPTOR SETS!\n");
			throw std::runtime_error("FAILED TO ALLOCATE DESCRIPTOR SETS!");
		}

#ifndef NDEBUG
		dbg::printNote("Descriptor Idx: %d - Successfully Allocated Descriptor Sets\n     Set Count: %d\n", descriptorsIdx, allocInfo.descriptorSetCount);
#endif // !NDEBUG

		// Loop through each frame
		for (size_t frameIdx = 0; frameIdx < _MAX_FRAMES_IN_FLIGHT; frameIdx++) {
			std::vector<VkWriteDescriptorSet> descriptorWrites;
			descriptorWrites.reserve(createInfo->shaders.size());

			// this vector is to stop data from going out of scope in the for loop.
			// outer vector is for shaders, inner vector is the image infos array
			std::vector<std::vector<VkDescriptorImageInfo>> imageInfos(createInfo->shaders.size());
			std::vector<std::vector<VkDescriptorImageInfo>> textureInfos(createInfo->shaders.size());

			size_t previousShaderUBOcount = 0;
			size_t previousShaderSSBOcount = 0;
			// Loop through shaders
			for (size_t shaderIdx = 0; shaderIdx < createInfo->shaders.size(); shaderIdx++) {

				auto& shader = createInfo->shaders[shaderIdx];


				// https://registry.khronos.org/vulkan/specs/latest/man/html/VkWriteDescriptorSet.html
				std::vector<VkWriteDescriptorSet> shaderDescriptorWrites
						= *createInfo->shaders[shaderIdx]->getDescriptorWrites();

				// buffer infos for descriptor writes
				// first vector is each frame in flight, second is for each UBO handle, third is for the count (i.e. array of UBOs)
				std::vector<std::vector<std::vector<VkDescriptorBufferInfo>>>& BFR_infos = *(shader->getDescriptorBufferInfos(*this));

				if (shader->_imageSamplers.size()>0) {
					imageInfos[shaderIdx].resize(shader->_imageSamplers.size());
				}

				if (shader->_textures.size() > 0) {
					textureInfos[shaderIdx].resize(0);
					//textureInfos[shaderIdx].resize(shader->_textures.size());
				}

				for (uint32_t k = 0; k < BFR_infos[frameIdx].size(); k++) {
					(shaderDescriptorWrites)[k].pBufferInfo = BFR_infos[frameIdx][k].data();
				}
			

				// loop through the descriptor writes from the shader
				for (size_t dw_idx/*DescriptorWriteIndex*/= 0; dw_idx < shaderDescriptorWrites.size(); dw_idx++) {
					VkWriteDescriptorSet& descriptorWrite = (shaderDescriptorWrites)[dw_idx];
					descriptorWrite.dstSet = _descriptorSets[descriptorsIdx][frameIdx];

					if (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER == descriptorWrite.descriptorType || VK_DESCRIPTOR_TYPE_SAMPLER == descriptorWrite.descriptorType) {
						// setup image infos for image samplers
						for (size_t j = 0; j < imageInfos[shaderIdx].size(); ++j) {
							#ifndef NDEBUG
							if (shader->_imageSamplers[j].values[0]->getImageView() == VK_NULL_HANDLE) {
								printf("VAL: WARNING: Each image sampler must be bound to a valid image view before the descriptor sets are updated!\n"
								"\tSampler at %h is not bound to a valid image view!\n", &shader->_imageSamplers[j].values[0]);
							}
							#endif // !NDEBUG

							imageInfos[shaderIdx][j] = shader->_imageSamplers[j].values[0]->getVkDescriptorImageInfo();
							// imageinfo for the respective sampler
							//imageInfos[shaderIdx][j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							//imageInfos[shaderIdx][j].imageView = *(shader->_imageSamplers[j].values[0]->getImageView());
							//imageInfos[shaderIdx][j].sampler = shader->_imageSamplers[j].values[0]->getVkSampler();
						}

						if (shader->_imageSamplers.size() > 0) {
							descriptorWrite.pImageInfo = imageInfos[shaderIdx].data();
						}
					}

					if (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE == descriptorWrite.descriptorType) {
						for (size_t j = 0; j < shader->_textures.size(); ++j) {
							for (size_t k = 0; k < shader->_textures[j].values.size(); ++k) {
								textureInfos[shaderIdx].resize(textureInfos[shaderIdx].size() + 1u);
								textureInfos[shaderIdx].back().imageLayout = (shader->_textures[j].values[k]->getLayout());
								textureInfos[shaderIdx].back().imageView = (shader->_textures[j].values[k]->getImageView());
								textureInfos[shaderIdx].back().sampler = NULL;
							}
						}
						if (shader->_textures.size() > 0) {
							descriptorWrite.pImageInfo = textureInfos[shaderIdx].data();
						}
					}
					// add descriptor write to the write list
					descriptorWrites.push_back(descriptorWrite);
					

				}

				previousShaderUBOcount += shader->_UBO_Handles.size();
				previousShaderSSBOcount += shader->_SSBO_Handles.size();
			}

#ifndef NDEBUG
			// Validates buffers
			for (auto& write : descriptorWrites) {
				if (write.pBufferInfo && write.pBufferInfo->buffer == VK_NULL_HANDLE) {
					printf("VAL: ERROR: BUFFER IN BUFFER INFO IS NULLPTR!\n");
					throw std::runtime_error("VAL: ERROR: BUFFER INFO IS NULLPTR!");
				}
			}
#endif // !NDEBUG

			// Update the descriptor sets with the accumulated descriptor writes
			/*
			 pDescriptorWrites[1].pImageInfo[0].imageView is VK_NULL_HANDLE.
			The Vulkan spec states: If descriptorType is VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, or VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			and the nullDescriptor feature is not enabled, the imageView member of each element of
			pImageInfo must not be VK_NULL_HANDLE (https://vulkan.lunarg.com/doc/view/1.3.296.0/windows/1.3-extensions/vkspec.html#VUID-VkWriteDescriptorSet-descriptorType-02997)
			*/
			// In other words, the Image Sampler must be created before the descriptor sets.
			vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
#ifndef NDEBUG
			printf("Descriptor Idx: %d - Updated Descriptor sets\n     Frame Index: %zd\n    Write Count: %zd\n", descriptorsIdx, frameIdx, descriptorWrites.size());
#endif // !NDEBUG
		}
	}

	void VAL_PROC::createGraphicsPipelines(std::vector<graphicsPipelineCreateInfo*>& createInfos) {
		
		static std::vector<VkGraphicsPipelineCreateInfo> pipelineInfos(createInfos.size());
		//static std::vector<VkPipelineLayout> pipelineLayouts(createInfos.size());
		_pipelineLayouts.resize(createInfos.size());
		static std::vector<std::vector<VkPipelineShaderStageCreateInfo>> shaderStages(createInfos.size());

		_graphicsPipelines.resize(createInfos.size());

		// The way that I'm managing memory here is dogwater and needs to be improved
		std::vector<void*> toBeFreed;
		std::vector<void*> toBeFreedButCreatedWithMalloc; // (mixing is irrelevant bc void* removes type info, so this didn't even make sense)  C & C++ memory management can't be mixed
		// note that most of these vectors could be created with malloc instead, and perhaps put into 1 big allocated list rather than many small ones
		std::vector<std::vector<VkPushConstantRange>*> pushConstantRangesToBeFreed;
		std::vector<std::vector<VkVertexInputBindingDescription>*> vertexInputBindingDescriptionsToBeFreed;
		std::vector<std::vector<VkVertexInputAttributeDescription>*> vertexAttributeDescriptionsToBeFreed;
		std::vector<std::vector<VkSpecializationMapEntry>*> VkSpecializationMapEntriesToBeFreed;
		std::vector<std::vector<VkPipelineColorBlendAttachmentState>*> colorBlendStateAttachmentsToBeFreed;

		for (int pidx = 0; pidx < createInfos.size(); ++pidx) {

			graphicsPipelineCreateInfo* createInfo = createInfos[pidx];
			shaderStages[pidx].resize(createInfo->shaders.size());

			// load pushConstantRanges
			std::vector<VkPushConstantRange>* pushConstantRanges = new std::vector<VkPushConstantRange>;
			pushConstantRangesToBeFreed.push_back(pushConstantRanges);
			for (pushConstantHandle* PC_Hdl : createInfo->getUniquePushConstants()) {
				pushConstantRanges->push_back(PC_Hdl->toVkPushConstantRange());
			}


			for (size_t i = 0; i < createInfo->shaders.size(); ++i) {
				VkPipelineShaderStageCreateInfo& shaderStageInfo = (shaderStages)[pidx][i];
				shader* shdr =
					createInfo->shaders[i];

				// load specialization constants
				uint32_t specializationInfoSize = 0u;
				std::vector<VkSpecializationMapEntry>* specializationEntries = new std::vector<VkSpecializationMapEntry>;
				specializationEntries->reserve(shdr->_specializationConstants.size());
				VkSpecializationMapEntriesToBeFreed.push_back(specializationEntries);

				// get the size in bytes to allocate to hold the list of specializationInfos
				for (size_t j = 0; j < shdr->_specializationConstants.size(); j++) {
					if (shdr->_specializationConstants[j].values.size() == 0) {
						#ifndef NDEBUG
						printf("VAL: WARNING: Specialization constant #%u of shader @ %h, #%u, of pipeline @ %h, #%d has an invalid value!\n", j, i, shdr, createInfo, pidx);
						#endif // !NDEBUG
						continue;
					}
					specializationEntries->resize(specializationEntries->size() + 1);
					specializationEntries->back() = *(shdr->_specializationConstants[j].values[0]);
					specializationInfoSize += (shdr->_specializationConstants[j].values[0])->_dataSize;
				}

				// create the list of specializationInfos
				void* specializationInfoData = malloc(specializationInfoSize);
				toBeFreedButCreatedWithMalloc.push_back(specializationInfoData);

				{ // indentation for scope restriction only
					uint32_t prevSize = 0;
					// write specialization data to specializationInfoData buffer
					for (size_t j = 0; j < shdr->_specializationConstants.size(); j++) {
						uint32_t dataSize = (shdr->_specializationConstants[j].values[0])->_dataSize;
						void* data = (shdr->_specializationConstants[j].values[0])->_data;
						for (uint32_t k = 0; k < dataSize; ++k) {
							((char*)(specializationInfoData))[k + prevSize] = ((char*)(data))[k];
						}

						prevSize += dataSize;
					}
				}


				VkSpecializationInfo* specializationInfo = new VkSpecializationInfo;
				toBeFreed.push_back(specializationInfo);

				specializationInfo->mapEntryCount = specializationEntries->size();
				specializationInfo->pMapEntries = specializationEntries->data();
				specializationInfo->dataSize = specializationInfoSize;
				specializationInfo->pData = specializationInfoData;
				//////////////////////////////////////////////////////////////////////
				// shader stage creation

				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = (VkShaderStageFlagBits)shdr->getStageFlags();
				shaderStageInfo.module = createShaderModule(shdr->getByteCode().data(), shdr->getByteCode().size());
				shaderStageInfo.pSpecializationInfo = specializationInfo;
				if (shaderStageInfo.module == VK_NULL_HANDLE) {
					printf("VAL: FAILED TO CREATE A SHADER MODULE!");
				}
				shaderStageInfo.pName = shdr->getEntryPoint().data();
			}

			// create vertex input info
			VkPipelineVertexInputStateCreateInfo* vertexInputInfo = new VkPipelineVertexInputStateCreateInfo;
			//0-init
			memset(vertexInputInfo, 0, sizeof(*vertexInputInfo));
			toBeFreed.push_back(vertexInputInfo);

			vertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;



			// create vector of vertex binding desciptions
			std::vector<VkVertexInputBindingDescription>* vertexBindingDescriptions = new std::vector<VkVertexInputBindingDescription>;
			vertexInputBindingDescriptionsToBeFreed.push_back(vertexBindingDescriptions);
			vertexBindingDescriptions->reserve(createInfo->shaders.size());

			std::vector<VkVertexInputAttributeDescription>* vertexAttributeDescriptions = new std::vector<VkVertexInputAttributeDescription>;
			vertexAttributeDescriptionsToBeFreed.push_back(vertexAttributeDescriptions);
			uint32_t locationOffset = 0; // Tracks the current location for shader attribute offset

			for (shader* shdr : createInfo->shaders) {
				if (shdr->getVertexAttributes().size() > 0) {
					vertexBindingDescriptions->insert(vertexBindingDescriptions->end(), 
						shdr->getBindingDescriptions().begin(), shdr->getBindingDescriptions().end());
					std::vector<VkVertexInputAttributeDescription> attributeDescriptions = shdr->getVertexAttributes();
					// Adjust locations to avoid overlap
					for (auto& attribute : attributeDescriptions) {
						attribute.location += locationOffset;
					}
					locationOffset += (uint32_t)(attributeDescriptions.size());
					vertexAttributeDescriptions->insert(vertexAttributeDescriptions->end(), attributeDescriptions.begin(), attributeDescriptions.end());
				}
			}

			// binding descriptions
			vertexInputInfo->vertexBindingDescriptionCount = vertexBindingDescriptions->size();
			vertexInputInfo->pVertexBindingDescriptions = vertexBindingDescriptions->data();

			// attribute descriptions
			vertexInputInfo->vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions->size());
			vertexInputInfo->pVertexAttributeDescriptions = vertexAttributeDescriptions->data();

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			VkPipelineInputAssemblyStateCreateInfo* inputAssembly = new VkPipelineInputAssemblyStateCreateInfo;
			memset(inputAssembly, 0, sizeof(*inputAssembly));
			toBeFreed.push_back(inputAssembly);
			inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly->topology = createInfo->_topology;
			inputAssembly->primitiveRestartEnable = VK_FALSE;

			// create viewport
			VkViewport* viewport = new VkViewport;
			toBeFreed.push_back(viewport);
			viewport->x = 0.0f;
			viewport->y = 0.0f;
			viewport->width = (float)_windowVAL->_swapChainExtent.width;
			viewport->height = (float)_windowVAL->_swapChainExtent.height;
			viewport->minDepth = 0.0f;
			viewport->maxDepth = 1.0f;

			// create scissor
			VkRect2D* scissor = new VkRect2D;
			toBeFreed.push_back(scissor);
			scissor->offset = { 0,0 };
			scissor->extent = _windowVAL->_swapChainExtent;

			// create viewport state
			VkPipelineViewportStateCreateInfo* viewportState = new VkPipelineViewportStateCreateInfo;
			memset(viewportState, 0, sizeof(*viewportState));
			toBeFreed.push_back(viewportState);
			viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState->viewportCount = 1;
			viewportState->pViewports = viewport;
			viewportState->scissorCount = 1;
			viewportState->pScissors = scissor;

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges->size();
			pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges->data();

			if (createInfo->hasPushDescriptorLayout()) {
				pipelineLayoutInfo.setLayoutCount = 2;
			}
			else {
				pipelineLayoutInfo.setLayoutCount = 1;
			}

			VkDescriptorSetLayout* setLayouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout)*pipelineLayoutInfo.setLayoutCount);
			toBeFreedButCreatedWithMalloc.push_back((void*)setLayouts);
			if (createInfo->hasPushDescriptorLayout()) {
				setLayouts[0] = _descriptorSetLayouts[pidx];
				setLayouts[1] = _pushDescriptorSetLayouts[getPushDescriptorIndexOfPipeline(createInfo)];
				createInfo->pushDescriptorsSetNo = 1;
			}
			else {
				setLayouts[0] = _descriptorSetLayouts[pidx];
			}

			pipelineLayoutInfo.pSetLayouts = setLayouts;


			const uint32_t dynamicStateCount = createInfo->_dynamicStates.size();
			VkDynamicState* dynamicStates = (VkDynamicState*)createInfo->_dynamicStates.data(); // VAL::DYNAMIC_STATE maps directly to VkDynamicState
			//(https://docs.vulkan.org/guide/latest/dynamic_state.html)
			// dynamic states can be used to dynamically modify parts of the
			// graphics pipeline at runtime (i.e. the size of the viewport and scissor)
			VkPipelineDynamicStateCreateInfo* dynamicState = new VkPipelineDynamicStateCreateInfo;
			// 0-init
			memset(dynamicState, 0, sizeof(*dynamicState));
			toBeFreed.push_back(dynamicState);

			dynamicState->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState->dynamicStateCount = static_cast<uint32_t>(dynamicStateCount);
			dynamicState->pDynamicStates = dynamicStates;

			if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayouts[pidx]) != VK_SUCCESS) {
				throw std::runtime_error("FAILED TO CREATE VULKAN PIPELINE LAYOUT!");
			}

			VkGraphicsPipelineCreateInfo& pipelineInfo = pipelineInfos[pidx];
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.renderPass = createInfos[pidx]->renderPass->getVkRenderPass();
			pipelineInfo.stageCount = shaderStages[pidx].size();
			pipelineInfo.subpass = createInfos[pidx]->subpassIndex;
			pipelineInfo.pStages = shaderStages[pidx].data();
			pipelineInfo.pVertexInputState = vertexInputInfo;
			pipelineInfo.pInputAssemblyState = inputAssembly;
			pipelineInfo.pViewportState = viewportState;
			
			VkPipelineMultisampleStateCreateInfo* multisampleState = new VkPipelineMultisampleStateCreateInfo;
			toBeFreed.push_back(multisampleState);
			*multisampleState = createInfo->getVkPipelineMultisampleStateCreateInfo();

			VkPipelineColorBlendStateCreateInfo* colorBlendState = new VkPipelineColorBlendStateCreateInfo;
			// 0-init
			memset(colorBlendState, 0, sizeof(*colorBlendState));
			toBeFreed.push_back(colorBlendState);

			if (createInfo->_colorBlendState != NULL) {
				std::vector<VkPipelineColorBlendAttachmentState>* colorBlendStateAttachments = new std::vector<VkPipelineColorBlendAttachmentState>;
				colorBlendStateAttachmentsToBeFreed.push_back(colorBlendStateAttachments);
				*colorBlendState =
					createInfo->_colorBlendState->toVkPipelineColorblendStateCreateInfo(colorBlendState, colorBlendStateAttachments);
				colorBlendState->pAttachments = colorBlendStateAttachments->data();
				colorBlendState->attachmentCount = colorBlendStateAttachments->size();
			}

			pipelineInfo.pRasterizationState = &(createInfo->_rasterizerState->_VKrasterizerState);
			pipelineInfo.pMultisampleState = multisampleState;
			pipelineInfo.pColorBlendState = colorBlendState;
			pipelineInfo.pDynamicState = dynamicState;
			pipelineInfo.layout = _pipelineLayouts[pidx];

			if (createInfo->_depthStencilState != NULL) {
				pipelineInfo.pDepthStencilState = (&(createInfo->_depthStencilState->_VKdepthState));
			}
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		}

		if (pipelineInfos.empty()) {
			dbg::printNote("Graphics pipeline create infos are empty.");
		} else {
			if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, pipelineInfos.size(), pipelineInfos.data(), nullptr, _graphicsPipelines.data()) != VK_SUCCESS)
			{
				dbg::printError("Failed to create graphics pipelines");
				throw std::runtime_error("VAL: FAILED TO CREATE GRAPHICS PIPELINES!");
			}
		}

		{
			// free allocated memory & cleanup shader modules
			for (size_t pidx = 0; pidx < pipelineInfos.size(); ++pidx) {
				for (auto& shaderStageInfo : shaderStages[pidx]) {
					vkDestroyShaderModule(_device, shaderStageInfo.module, NULL);
				}
			}

			// free allocated memory
			for (uint32_t i = 0; i < toBeFreed.size(); ++i) {
				delete (toBeFreed[i]);
			}

			// free allocated memory
			for (uint32_t i = 0; i < toBeFreedButCreatedWithMalloc.size(); ++i) {
				free(toBeFreedButCreatedWithMalloc[i]);
			}


			// free allocated memory
			for (auto vec : colorBlendStateAttachmentsToBeFreed) {
				delete vec;
			}

			// free allocated memory
			for (auto vec : pushConstantRangesToBeFreed) {
				delete vec;
			}

			// free allocated memory
			for (auto vec : vertexInputBindingDescriptionsToBeFreed) {
				delete vec;
			}

			// free allocated memory
			for (auto vec : vertexAttributeDescriptionsToBeFreed) {
				delete vec;
			}

			// free allocated memory
			for (auto vec : VkSpecializationMapEntriesToBeFreed) {
				delete vec;
			}
		}
	}

	void VAL_PROC::createComputePipelines(std::vector<computePipelineCreateInfo*>& createInfos) {
		static std::vector<VkComputePipelineCreateInfo> pipelineInfos(createInfos.size());

		_computePipelines.resize(createInfos.size());
		_computePipelineLayouts.resize(createInfos.size());
		static std::vector<std::vector<VkPipelineShaderStageCreateInfo>> shaderStages(createInfos.size());

		std::vector<void*> toBeFreed;
		std::vector<void*> toBeFreedButCreatedWithMalloc; // C & C++ memory management can't be mixed
		for (int pidx = 0; pidx < createInfos.size(); ++pidx) {


			computePipelineCreateInfo* createInfo = createInfos[pidx];
			shaderStages[pidx].resize(createInfo->shaders.size());

#ifndef NDEBUG
			if (createInfo->shaders.size() > 1) {
				dbg::printError("Compute pipelines only allow for 1 shader, but compute pipeline %h has %zu shaders", &createInfo, createInfo->shaders.size());
			}
#endif // !NDEBUG

			VkPipelineShaderStageCreateInfo& shaderStageInfo = (shaderStages)[pidx][0];
			shader* shdr =
				createInfo->shaders[0]; // there can only be 1 shader in a compute pipeline

			// load pushConstantRanges
			std::vector<VkPushConstantRange>* pushConstantRanges = new std::vector<VkPushConstantRange>;
			toBeFreed.push_back(pushConstantRanges);
			for (pushConstantHandle* PC_Hdl : createInfo->getUniquePushConstants()) {
				pushConstantRanges->push_back(PC_Hdl->toVkPushConstantRange());
			}
			
			// load specialization constants
			uint32_t specializationInfoSize = 0u;
			std::vector<VkSpecializationMapEntry>* specializationEntries = new std::vector<VkSpecializationMapEntry>;
			specializationEntries->reserve(shdr->_specializationConstants.size());
			toBeFreed.push_back(specializationEntries);
			for (size_t j = 0; j < shdr->_specializationConstants.size(); j++) {
				specializationEntries->resize(specializationEntries->size()+1);
				specializationEntries->back() = *(shdr->_specializationConstants[j].values[0]);
				specializationInfoSize += (shdr->_specializationConstants[j].values[0])->_dataSize;
			}
			void* specializationInfoData = malloc(specializationInfoSize);

			{ // indentation for scope restriction only
				uint32_t prevSize = 0;
				// write specialization data to specializationInfoData buffer
				for (size_t j = 0; j < shdr->_specializationConstants.size(); j++) {
					uint32_t dataSize = (shdr->_specializationConstants[j].values[0])->_dataSize;
					void* data = (shdr->_specializationConstants[j].values[0])->_data;
					for (uint32_t k = 0; k < dataSize; ++k) {
						((char*)(specializationInfoData))[k + prevSize] =
							((char*)(data))[k];
					}

					prevSize += dataSize;
				}
			}

			toBeFreedButCreatedWithMalloc.push_back(specializationInfoData);

			VkSpecializationInfo* specializationInfo = new VkSpecializationInfo;
			toBeFreed.push_back(specializationInfo);

			specializationInfo->mapEntryCount = specializationEntries->size();
			specializationInfo->pMapEntries = specializationEntries->data();
			specializationInfo->dataSize = specializationInfoSize;
			specializationInfo->pData = specializationInfoData;
			//////////////////////////////////////////////////////////////////////
			// shader stage creation
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			shaderStageInfo.pSpecializationInfo = specializationInfo;
			shaderStageInfo.module = createShaderModule(shdr->getByteCode().data(), shdr->getByteCode().size());

			if (shaderStageInfo.module == VK_NULL_HANDLE) {
				printf("VAL: FAILED TO CREATE A SHADER MODULE!");
			}
			shaderStageInfo.pName = shdr->getEntryPoint().data();

			///////////////////////////////////////
			// create pipeline layout
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges->size();
			pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges->data();

			if (createInfo->hasPushDescriptorLayout()) {
				pipelineLayoutInfo.setLayoutCount = 2;
			}
			else {
				pipelineLayoutInfo.setLayoutCount = 1;
			}

			VkDescriptorSetLayout* setLayouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) * pipelineLayoutInfo.setLayoutCount);

			if (createInfo->hasPushDescriptorLayout()) {
				setLayouts[0] = _descriptorSetLayouts[_graphicsPipelines.size() + pidx];
				setLayouts[1] = _pushDescriptorSetLayouts[getPushDescriptorIndexOfPipeline(createInfo)];
			}
			else {
				setLayouts[0] = _descriptorSetLayouts[_graphicsPipelines.size() + pidx];
			}

			pipelineLayoutInfo.pSetLayouts = setLayouts;
			toBeFreedButCreatedWithMalloc.push_back((void*)setLayouts);

			// add pipeline info to create list
			VkComputePipelineCreateInfo& pipelineInfo = pipelineInfos[pidx];
			pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			pipelineInfo.stage = shaderStageInfo;
			pipelineInfo.layout = _computePipelineLayouts[pidx];
		}



		if (pipelineInfos.empty()) {
			dbg::printNote("VAL: NOTE: Compute pipelines create infos are empty.");
		}
		else {
			if (vkCreateComputePipelines(_device, VK_NULL_HANDLE, pipelineInfos.size(), pipelineInfos.data(), nullptr, _computePipelines.data()) != VK_SUCCESS) {
				dbg::printError("Failed to create compute pipelines");
				throw std::runtime_error("VAL: FAILED TO CREATE GRAPHICS PIPELINES!");
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////

		// cleanup shader modules
		for (size_t pidx = 0; pidx < pipelineInfos.size(); ++pidx) {
			for (auto& shaderStageInfo : shaderStages[pidx]) {
				vkDestroyShaderModule(_device, shaderStageInfo.module, NULL);
			}
		}
		// free allocated memory
		for (uint32_t i = 0; i < toBeFreed.size(); ++i) {
			delete (toBeFreed[i]);
		}
		// free allocated memory
		for (uint32_t i = 0; i < toBeFreedButCreatedWithMalloc.size(); ++i) {
			free(toBeFreedButCreatedWithMalloc[i]);
		}
	}

	void VAL_PROC::createCommandPool() {
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _graphicsQueue._queueFamily;

		if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("VAL: failed to create graphics command pool!");
		}
	}

	void VAL_PROC::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE BUFFER MEMORY!");
		}

		vkBindBufferMemory(_device, buffer, bufferMemory, 0);
	}

	void VAL_PROC::createUniformBuffers(std::vector<pipelineCreateInfo*> createInfos) {
		std::vector<UBO_Handle*> uniqueUBOs;
		for (size_t i = 0; i < createInfos.size(); ++i) {
			pipelineCreateInfo* pipelineInfo = createInfos[i];
			std::vector<UBO_Handle*> uniqueUBOsOfThisPipelineInfo = pipelineInfo->getUniqueUBOs();
			for (UBO_Handle* UBO_HDL : uniqueUBOsOfThisPipelineInfo) {
				if (std::find(uniqueUBOs.begin(), uniqueUBOs.end(), UBO_HDL) == uniqueUBOs.end()) {
					uniqueUBOs.push_back(UBO_HDL);
				}
			}
		}

		_uboArr.create(*this, uniqueUBOs.data(), uniqueUBOs.size());
	}

	void VAL_PROC::createSSBOs(std::vector<pipelineCreateInfo*> createInfos) {
		// Resize the outer vector for the number of frames
		// Resize the outer vector for the number of frames
		_SSBO_Buffers.resize(_MAX_FRAMES_IN_FLIGHT);
		_SSBO_Memory.resize(_MAX_FRAMES_IN_FLIGHT);
		_SSBO_DataMapped.resize(_MAX_FRAMES_IN_FLIGHT);

		// some shaders may reference the same UBO, 
		// therefore it is nessecary to deduplicate references to UBOs
		// to allow them to be updated and modified via a common index
		std::vector<SSBO_Handle*> uniqueSSBOs;
		for (size_t i = 0; i < createInfos.size(); ++i) {
			pipelineCreateInfo* pipelineInfo = createInfos[i];
			std::vector<SSBO_Handle*> uniqueUBOsOfThisPipelineInfo = pipelineInfo->getUniqueSSBOs();
			for (SSBO_Handle* SSBO_HDL : uniqueUBOsOfThisPipelineInfo) {
				if (std::find(uniqueSSBOs.begin(), uniqueSSBOs.end(), SSBO_HDL) == uniqueSSBOs.end()) {
					uniqueSSBOs.push_back(SSBO_HDL);
				}
			}
		}

		for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
			_SSBO_Buffers[i].resize(uniqueSSBOs.size());
			_SSBO_Memory[i].resize(uniqueSSBOs.size());
			_SSBO_DataMapped[i].resize(uniqueSSBOs.size());

			for (size_t j = 0; j < uniqueSSBOs.size(); ++j) {
				SSBO_Handle* SSBO = uniqueSSBOs[j];
				SSBO->_index = j;

				createBuffer(SSBO->_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | SSBO->_additionalUsageFlags, SSBO->getMemoryPropertyFlags(),
					_SSBO_Buffers[i][SSBO->_index], _SSBO_Memory[i][SSBO->_index]);

				if (SSBO->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
					// Map the buffer for the current shader
					vkMapMemory(_device, _SSBO_Memory[i][SSBO->_index],
						0, SSBO->_size, 0, &_SSBO_DataMapped[i][SSBO->_index]);
				}
			}
		}
	}

	void VAL_PROC::createImage(const uint32_t& width, const uint32_t& height, const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, 
		VkImage& image, VkDeviceMemory& imageMemory, const uint8_t& mipMapLevel /*DEFAULT=1*/, const VkSampleCountFlagBits& NumSamples /*DEFAULT: VK_SAMPLE_COUNT_1_BIT*/, const VkImageLayout initialLayout /*DEFAULT: VK_IMAGE_LAYOUT_UNDEFINED*/)
	{

#ifndef NDEBUG
		if (NumSamples == 0x0) {
			dbg::printError("Cannot create an image with a sample count of %d!", NumSamples);
			throw std::runtime_error("VAL: ERROR: Cannot create an image with a sample count of 0!");
		}
#endif // !NDEBUG

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipMapLevel;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; /*The Vulkan spec states : initialLayout must be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED(https ://vulkan.lunarg.com/doc/view/1.4.309.0/windows/antora/spec/latest/chapters/resources.html#VUID-VkImageCreateInfo-initialLayout-00993)*/
		imageInfo.usage = usage;
		imageInfo.samples = NumSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
#ifndef NDEBUG
		if (vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("VAL: FAILED TO CREATE VK IMAGE!");
		}
#else
		vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS
#endif

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO ALLOCATE VK IMAGE MEMORY!");
		}

		vkBindImageMemory(_device, image, imageMemory, 0);


#ifndef NDEBUG
		if (initialLayout == VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL /*and not dynamic rendering enabled*/) {
			dbg::printError("An intial layout of VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL is not allowed unless the VK_KHR_dynamic_rendering extension is enabled."
				"Refer to the Vulkan specification for more details : https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_dynamic_rendering.html, https://docs.vulkan.org/features/latest/features/proposals/VK_KHR_dynamic_rendering.html");
		}
#endif // !NDEBUG

		if (initialLayout != VK_IMAGE_LAYOUT_UNDEFINED) {

			transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, initialLayout, NULL, mipMapLevel);
		}
	}

	uint32_t VAL_PROC::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("FAILED TO FIND A SUITABLE MEMORY TYPE!");
	}

	inline void setTransitionLayoutImageInfo(const VkImageLayout& layout, VkAccessFlags& accessMask, VkPipelineStageFlags& stageBit) {
		switch (layout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			accessMask = 0;
			stageBit = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			accessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			stageBit = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			accessMask = VK_ACCESS_SHADER_READ_BIT;
			stageBit = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			accessMask = VK_ACCESS_TRANSFER_READ_BIT;
			stageBit = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			stageBit = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;

		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			accessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			stageBit = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;

		default:
			dbg::printError("Unsupported image transition.");
			throw std::logic_error("VAL: UNSUPPORTED TYPE FOR IMAGE TRANSITION!");
			break;
		}
	}

	// a helper function used to change the layout of an image
	void VAL_PROC::transitionImageLayout(VkImage image, const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout,
		VkCommandBuffer commandBuffer /*NULL BY DEFAULT*/, const uint32_t& mipLevels/*DEFAULT=1U*/) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		//barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		VkPipelineStageFlags destinationStage;

		setTransitionLayoutImageInfo(oldLayout, barrier.srcAccessMask, sourceStage);
		setTransitionLayoutImageInfo(newLayout, barrier.dstAccessMask, destinationStage);

		bool usingSingleTimeCommandBuffer = false;
		if (commandBuffer == NULL) {
			commandBuffer =
				beginSingleTimeCommands();
			usingSingleTimeCommandBuffer = true;
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		if (usingSingleTimeCommandBuffer) {
			endSingleTimeCommands(commandBuffer);
		}
		//if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		//	barrier.srcAccessMask = 0;
		//	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;  // The image was being read in shaders
		//	barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // Now it will be written to as a color attachment

		//	sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // The previous stage was the fragment shader (reading the image)
		//	destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  // The next stage is rendering to the color attachment
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		//	barrier.srcAccessMask = 0;
		//	barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		//	barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		//	barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		//	sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		//	destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		//	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;  // The image was being read in shaders
		//	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;  // Now it will be written to in a transfer operation

		//	sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // The previous stage was the fragment shader (reading)
		//	destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;  // The next stage is a transfer operation (writing)
		//}
		//else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		//	barrier.srcAccessMask = 0;  // No prior access to the image (because it's UNDEFINED)
		//	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;  // Access required for reading in shaders

		//	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;  // No previous operations to depend on
		//	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // The operation is going to the fragment shader
		//}
		//else {
		//	throw std::invalid_argument("unsupported layout transition!");
		//}
	}

	VkCommandBuffer VAL_PROC::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VAL_PROC::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_graphicsQueue._queue, 1, &submitInfo, VK_NULL_HANDLE);
		/*vkQueueWaitIdle is a Vulkan function that blocks execution of the CPU
		until all submitted commands on a specified queue have been completed by the GPU.*/
		vkQueueWaitIdle(_graphicsQueue._queue);

		vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
	}

	void VAL_PROC::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, const uint32_t& mipMapLevel /*DEFAULT=0U*/) {
#ifndef NDEBUG
		if (mipMapLevel == 0) {
			printf("VAL: A mipmap level of %d is invalid, the mipmap level must be greater than or equal to 1.\n", mipMapLevel);
		}
#endif // !NDEBUG

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;

		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipMapLevel;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imgViewRet;
		if (vkCreateImageView(_device, &viewInfo, nullptr, &imgViewRet) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		(*imageView) = imgViewRet;
	}

	void VAL_PROC::copyBufferToImage(VkBuffer buffer, VkImage image, const uint32_t& width, const uint32_t& height) {
#ifndef NDEBUG
		if (height == 0 or width == 0) {
			std::cout << "COPYING A BUFFER TO AN IMAGE WITH A HEIGHT OR WIDTH OF ZERO IS INVALID AND WILL RESULT IN MEMORY CORRUPTION!";
		}
#endif // !NDEBUG

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		endSingleTimeCommands(commandBuffer);
	}

	void VAL_PROC::generateMipMaps(VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipMapLevels) {
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(_physicalDevice, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipMapLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = std::clamp(mipMapLevels - 1, 0u, UINT32_MAX);
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}

	// copies the srcbuffer into the dstBuffer
	void VAL_PROC::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkDeviceSize srcOffset, const VkDeviceSize dstOffset) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = srcOffset; // optional
		copyRegion.dstOffset = dstOffset; // optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		// immediately executes command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_graphicsQueue._queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphicsQueue._queue);

		/*
		A fence would allow you to schedule multiple transfers simultaneously and wait for all of them complete,
		instead of executing one at a time. That may give the driver more opportunities to optimize.
		*/

		vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
	}

	VkShaderModule VAL_PROC::createShaderModule(const char* bytecode, const size_t bytecodeSize) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = bytecodeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode);

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE VK SHADER MODULE");
		}

		return shaderModule;
	}

	void VAL_PROC::copyImage(VkImage& src, VkImage& dst, const VkFormat& imageFormat, const VkImageLayout& srcLayout, const VkImageLayout dstLayout, const uint32_t& width, const uint32_t& height) {
		transitionImageLayout(src, imageFormat, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		transitionImageLayout(dst, imageFormat, dstLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Begin a command buffer for recording
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		// Define the region to copy
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.mipLevel = 0;
		copyRegion.srcSubresource.baseArrayLayer = 0;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.srcOffset = { 0, 0, 0 };

		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.mipLevel = 0;
		copyRegion.dstSubresource.baseArrayLayer = 0;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.dstOffset = { 0, 0, 0 };

		copyRegion.extent.width = width;
		copyRegion.extent.height = height;
		copyRegion.extent.depth = 1;

		// Record the copy command
		vkCmdCopyImage(
			commandBuffer,
			src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &copyRegion
		);

		endSingleTimeCommands(commandBuffer);

		transitionImageLayout(src, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcLayout);

		transitionImageLayout(dst, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstLayout);
	}

	uint32_t VAL_PROC::getMaxMipmapLevel() {
		VkSampleCountFlags counts = _physicalDeviceProperties.limits.framebufferColorSampleCounts & _physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	VkSampleCountFlagBits VAL_PROC::getMaxSampleCount() {
		VkSampleCountFlags counts = _physicalDeviceProperties.limits.framebufferColorSampleCounts & _physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	////////////////////////////////////////////////////////////////////////////////////////////

	void VAL_PROC::createVertexBuffer(const void* vertices, const size_t vertexCount,
		const size_t sizeOfVertex, VkBuffer* vertexBufferOut, VkDeviceMemory* vertexBufferMemoryOut, VkBufferUsageFlags additionalUsages /*DEFAULT=0*/) {
#ifndef NDEBUG
		if (!vertexBufferOut || !vertexBufferMemoryOut) {
			throw std::exception("indexBufferOut AND indexBufferMemoryOut MUST BOTH BE VALID POINTERS AND NOT FALL OUT OF SCOPE DURING RUNTIME!");
		}
#endif // !NDEBUG

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeOfVertex * vertexCount;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // buffer purpose
		bufferInfo.usage = bufferInfo.usage | additionalUsages;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, vertexBufferOut) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE VERTEX BUFFER!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, *vertexBufferOut, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, vertexBufferMemoryOut) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO ALLOCATE VERTEX BUFFER MEMORY!");
		}

		vkBindBufferMemory(_device, *vertexBufferOut, *vertexBufferMemoryOut, 0);

		// fills the vertex buffer and maps memory
		void* data;
		vkMapMemory(_device, *vertexBufferMemoryOut, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices, (size_t)bufferInfo.size);
		vkUnmapMemory(_device, *vertexBufferMemoryOut); // unmap to give flush memory to the GPU and give back Vulkan control over the buffer
	}

	void VAL_PROC::createIndexBuffer(uint32_t* indices, const uint32_t& indexCount, VkBuffer* indexBufferOut, VkDeviceMemory* indexBufferMemoryOut) {
#ifndef NDEBUG
		if (!indexBufferOut || !indexBufferMemoryOut) {
			dbg::printError("indexBufferOut (%p) and indexBufferMemoryOut (%p) must both be valid pointers and not fall out of scope during runtime.", indexBufferOut, indexBufferMemoryOut);
			throw std::exception("VAL: ERROR: indexBufferOut AND indexBufferMemoryOut MUST BOTH BE VALID POINTERS AND NOT FALL OUT OF SCOPE DURING RUNTIME!");
		}
#endif // !NDEBUG

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		// A staging buffer is a buffer used to transfer memory from the CPU to the GPU
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(_device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			*indexBufferOut, *indexBufferMemoryOut);

		copyBuffer(stagingBuffer, *indexBufferOut, bufferSize);

		vkDestroyBuffer(_device, stagingBuffer, nullptr);
		vkFreeMemory(_device, stagingBufferMemory, nullptr);
	}

	void VAL_PROC::cleanupTmpDeviceExtensions() {
		if (_deviceExtensions) {
			for (uint8_t i = 0; i < _deviceExtCount; ++i) {
				free(_deviceExtensions[i]);
			}
			free(_deviceExtensions);
			_deviceExtCount = 0u;
			_deviceExtensions = NULL;
		}
	}
}