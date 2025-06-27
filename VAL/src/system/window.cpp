#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void Window::setWindowHandleGLFW(GLFWwindow* window) {
		_window = window;
	}

	GLFWwindow* Window::getWindowHandleGLFW() {
		return _window;
	}

	void Window::configure(GLFWwindow* windowHDL, VkColorSpaceKHR colorSpace) {
		_window = windowHDL;
		_colorSpace = colorSpace;
	}

	void Window::display(const VkFormat& imageFormat, std::vector<VkSemaphore> waitOn) {
		vkWaitForFences(_procVAL->_device, 1, &_presentFence, VK_TRUE, UINT64_MAX);
		updateSwapChain(imageFormat, waitOn);
	}

	void Window::destroy() {
		cleanup();
	}

	void Window::cleanup() {
		if (_procVAL) {
			if (_ownsGLFWwindow && _window) {
				glfwDestroyWindow(_window);
				_window = NULL;
			}
			_presentQueue.~Queue();

			cleanupSwapChain();

			if (_surface) {
				vkDestroySurfaceKHR(_procVAL->_instance, _surface, NULL);
			}

			if (_presentFence)
			{
				vkDestroyFence(_procVAL->getVkLogicalDevice(), _presentFence, NULL);
			}

			_surface = NULL;
			_presentFence = NULL;

			_swapChainAttachmentCount = 0u;
			_swapChainImageCount = 0u;
		}
	}


	void Window::cleanupSwapChain() {
		if (_swapChainFramebuffers && _swapChainImageViews) {
			for (uint8_t i = 0; i < _swapChainImageCount; ++i)
			{
				vkDestroyFramebuffer(_procVAL->getVkLogicalDevice(), _swapChainFramebuffers[i], VK_NULL_HANDLE);
				vkDestroyImageView(_procVAL->getVkLogicalDevice(), _swapChainImageViews[i], VK_NULL_HANDLE);
			}

			free(_swapChainFramebuffers);
			free(_swapChainImageViews);
			_swapChainFramebuffers = NULL;
			_swapChainImageViews = NULL;
		}
		if (_swapChain) {
			vkDestroySwapchainKHR(_procVAL->getVkLogicalDevice(), _swapChain, VK_NULL_HANDLE);
		}

		if (_swapChainImages != VK_NULL_HANDLE)
		{ // prevent mem leak
			free(_swapChainImages);
			_swapChainImages = NULL;
		}

		_swapChainAttachmentCount = 0u;
		_swapChainImageCount = 0u;
		_swapChain = NULL;
	}


	void Window::createPresentQueue()
	{
		_presentQueue._proc = _procVAL;
		_presentQueue.create();
	}


	void Window::createSwapChain(const VkFormat swapchainFormat)
	{
#ifndef NDEBUG
		if (_swapChain != VK_NULL_HANDLE)
		{
			dbg::printWarning("createSwapChain was called on window %h that already has an initialized swapchain, calling this function more than once may lead to undefined behavior or program crashes.", this);
		}
#endif // !NDEBUG

		swapChainSupportDetails swapChainSupport = querySwapChainSupport(_procVAL->_physicalDevice, _surface);

		VkSurfaceFormatKHR surfaceFormat{};
		surfaceFormat.format = swapchainFormat;
		surfaceFormat.colorSpace = _colorSpace;

		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(_window, swapChainSupport.capabilities);


		// check how many swap chain images are allowed
		uint32_t minSwapchainImageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (minSwapchainImageCount > swapChainSupport.capabilities.maxImageCount) {
			minSwapchainImageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;

		createInfo.minImageCount = minSwapchainImageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(_procVAL->_physicalDevice, _surface);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(_procVAL->_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}


		uint32_t swapChainImageCount = 0u;
		// create swap chain images
		VkResult res = vkGetSwapchainImagesKHR(_procVAL->_device, _swapChain, &swapChainImageCount, VK_NULL_HANDLE);
		if (res != VK_SUCCESS || swapChainImageCount == 0u) {
			dbg::printError("Failed to query swapChainImageCount of Window %p, with a swapChainImageCount of %u. VkResult as a U32: ", this, swapChainImageCount, (uint32_t)res);
		}

		if (_swapChainImages != VK_NULL_HANDLE)
		{ // prevent mem leak
			free(_swapChainImages);
		}

		_swapChainImages = (VkImage*)malloc(sizeof(VkImage) * swapChainImageCount);

		res = vkGetSwapchainImagesKHR(_procVAL->_device, _swapChain, &swapChainImageCount, _swapChainImages);
		if (res != VK_SUCCESS)
		{
			dbg::printError("Failed to query swapChainImageCount of Window %p. VkResult as a U32: ", this, (uint32_t)res);
		}

		_swapChainImageCount = swapChainImageCount;
		_swapChainExtent = extent;

		createSwapChainImageViews(swapchainFormat);
	}




	void Window::recreateSwapChain(const VkFormat swapchainFormat) {
		int width = 0, height = 0;
		glfwGetFramebufferSize(_window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(_window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_procVAL->_device);

		cleanupSwapChain();

		createSwapChain(swapchainFormat);
		createSwapChainImageViews(swapchainFormat);
		createSwapChainFrameBuffers(_swapChainAttachments, _swapChainAttachmentCount, _swapChainRenderPass, _procVAL->_device);
	}

	void Window::createSwapChainFrameBuffers(VkImageView* Attachments, const uint16_t& attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice)
	{
		_swapChainAttachments = Attachments;
		_swapChainAttachmentCount = attachmentCount;
		_swapChainRenderPass = renderPass;


		if (_swapChainImageCount > 0) {
			// allocate swap chain image views
			VkFramebuffer* tmp = (VkFramebuffer*)realloc(_swapChainFramebuffers, sizeof(VkFramebuffer) * _swapChainImageCount);
			if (tmp == NULL) {
				if (_swapChainFramebuffers) {
					free(_swapChainFramebuffers);
					_swapChainFramebuffers = NULL;
				}
				dbg::printError("Failed to allocate frambuffers of Window @ %p: out of system memory.", this);
				return;
			}
			else {
				_swapChainFramebuffers = tmp;
			}

			for (size_t i = 0; i < _swapChainImageCount; i++) {
				//std::vector<VkImageView> attachmentsV = { _swapChainImageViews[i] };
				std::vector<VkImageView> attachmentsV;
				attachmentsV.resize(attachmentsV.size() + attachmentCount);
				for (int i = 0; i < attachmentCount; ++i) {
					attachmentsV[i] = Attachments[i];
				}
				attachmentsV.push_back(_swapChainImageViews[i]);

				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = attachmentsV.size();
				framebufferInfo.pAttachments = attachmentsV.data();
				framebufferInfo.width = _swapChainExtent.width;
				framebufferInfo.height = _swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
					throw std::runtime_error("FAILED TO CREATE FRAME BUFFER!");
				}
			}
		}
	}

	void Window::updateSwapChain(const VkFormat& imageFormat, std::vector<VkSemaphore>& waitOn) {

		const auto& currentFrame = _procVAL->_currentFrame;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		// wait on the signalSemaphores to signaled
		presentInfo.waitSemaphoreCount = waitOn.size();
		presentInfo.pWaitSemaphores = waitOn.data();

		VkSwapchainKHR swapChains[] = { _swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &_currentSwapChainImageIndex;

		VkResult result = vkQueuePresentKHR(_presentQueue.getVkQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _procVAL->_frameBufferResized) {
			_procVAL->_frameBufferResized = false;
			recreateSwapChain(imageFormat);
		}
		else if (result != VK_SUCCESS) {
			printf("VAL: FAILED TO PRESENT SWAPCHAIN IMAGE\n");
			throw std::runtime_error("VAL: FAILED TO PRESENT SWAPCHAIN IMAGE");
		}
	}

	void Window::waitForFences() {
		vkWaitForFences(_procVAL->_device, 1, &_presentFence, VK_TRUE, UINT64_MAX);
	}

	VkFramebuffer& Window::getSwapchainFramebuffer(const VkFormat& imageFormat) {
		//vkWaitForFences(_procVAL->_device, 1, &_presentQueue._fences[_procVAL->_currentFrame], VK_TRUE, UINT64_MAX);

		//uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_procVAL->_device, _swapChain, UINT64_MAX,
			_presentQueue.getSemaphore(_procVAL->_currentFrame), VK_NULL_HANDLE, &_currentSwapChainImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			vkDeviceWaitIdle(_procVAL->_device);
			recreateSwapChain(imageFormat);
			return _swapChainFramebuffers[_currentSwapChainImageIndex];
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("VAL: Failed to acquire swap chain image!");
		}

		return _swapChainFramebuffers[_currentSwapChainImageIndex];
	}

	// returns a swapchain frame buffer to use
	VkFramebuffer& Window::beginDraw(const VkFormat& imageFormat) {
#ifndef NDEBUG
		if (_presentFence == NULL) {
			dbg::printError("Cannot begin draw, Window @ %p is missing its present fence.", this);
			throw std::runtime_error("Cannot begin draw, Window is missing its present fence.");
		}
#endif // !NDEBUG

		vkResetFences(_procVAL->_device, 1, &_presentFence);
		VkFramebuffer& framebuffer = getSwapchainFramebuffer(imageFormat); // gets the swapchain framebuffer to be rendered to
		return framebuffer;
	}






	void Window::createPresentFence(VkDevice device)
	{
		const VkFenceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};

		if (vkCreateFence(device, &createInfo, NULL, &_presentFence)!=VK_SUCCESS) 
		{
			dbg::printError("Failed to create present fence of Window @ %p", this);
#ifndef NDEBUG
			throw std::runtime_error("Failed to create present fence.");
#endif // !NDEBUG
		}
	}

	void Window::createWindowSurface(VkInstance instance) {
		if (_surface) {
			return;
		}
		if (glfwCreateWindowSurface(instance, _window, nullptr, &_surface) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
		}
	}

	void Window::createSwapChainImageViews(const VkFormat swapchainFormat) {
		if (_swapChainImages == NULL) {
			dbg::printWarning("Attempting to create the swapchain image views of Window @ %p but there are no swapchain images.", this);
		}

		// allocate swap chain image views
		VkImageView* tmp = (VkImageView*)realloc(_swapChainImageViews, sizeof(VkImageView) * _swapChainImageCount);
		if (tmp == NULL) {
			dbg::printError("Failed to allocate image views of Window @ %p: out of system memory.", this);
			return;
		} else {
			_swapChainImageViews = tmp;
		}
		 
		for (size_t i = 0; i < _swapChainImageCount; i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapchainFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			_swapChainImageViews[i] = createImageView(_procVAL->_device, _swapChainImages[i], swapchainFormat);
		}
	}
}