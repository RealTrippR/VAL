#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void window::setWindowHandleGLFW(GLFWwindow* window) {
		_window = window;
	}

	GLFWwindow* window::getWindowHandleGLFW() {
		return _window;
	}

	void window::createWindowSurface(VkInstance instance) {
		if (_surface) {
			return;
		}
		if (glfwCreateWindowSurface(instance, _window, nullptr, &_surface) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
		}
	}

	void window::configure(GLFWwindow* windowHDL, VkColorSpaceKHR colorSpace) {
		_window = windowHDL;
		_colorSpace = colorSpace;
	}

	void window::display(const VkFormat& imageFormat, std::vector<VkSemaphore> waitOn) {
		vkWaitForFences(_procVAL->_device, 1, &(_presentQueue._fences[_procVAL->_currentFrame]), VK_TRUE, UINT64_MAX);
		updateSwapChain(imageFormat, waitOn);
		//vkResetFences(_procVAL->_device, 1, &(_presentQueue._fences[_procVAL->_currentFrame]));
	}

	void window::cleanup() {
		if (_procVAL) {
			if (_ownsGLFWwindow) {
				glfwDestroyWindow(_window);
			}
			_presentQueue.destroy(*_procVAL);
			cleanupSwapChain();
			vkDestroySurfaceKHR(_procVAL->_instance, _surface, NULL);
			_swapChain = NULL;
			_swapChainImageViews.clear();
			_swapChainFrameBuffers.clear();

			_procVAL = NULL;
		}
	}


	void window::cleanupSwapChain() {
		for (auto framebuffer : _swapChainFrameBuffers) {
			vkDestroyFramebuffer(_procVAL->_device, framebuffer, nullptr);
		}
		for (auto imageView : _swapChainImageViews) {
			vkDestroyImageView(_procVAL->_device, imageView, nullptr);
		}

		if (_swapChain) {
			vkDestroySwapchainKHR(_procVAL->_device, _swapChain, nullptr);
		}
	}


	void window::createSwapChain(const VkFormat swapchainFormat) 
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

		//surfaceFormat.format = findSurfaceImageFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(_window, swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		queueFamilyIndices indices = findQueueFamilies(_procVAL->_physicalDevice, _surface);
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

		//vkGetSwapchainImagesKHR(_procVAL->_device, _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_procVAL->_device, _swapChain, &imageCount, _swapChainImages.data());

		_swapChainExtent = extent;
	}

	void window::recreateSwapChain(const VkFormat swapchainFormat) {
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
		createSwapChainFrameBuffers(_swapChainExtent, _swapChainAttachments, _swapChainAttachmentCount, _swapChainRenderPass, _procVAL->_device);
		//_procFML->createFrameBuffers(_swapChainExtent);
		//createSyncObjects();
	}

	void window::createSwapChainImageViews(const VkFormat swapchainFormat) {
		_swapChainImageViews.resize(_swapChainImages.size());

		for (size_t i = 0; i < _swapChainImages.size(); i++) {
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

			_swapChainImageViews[i] = createImageView(_procVAL->_device, +_swapChainImages[i], swapchainFormat);
		}
	}

	void window::createSwapChainFrameBuffers(const VkExtent2D& extent, VkImageView* Attachments, const uint16_t& attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice)
	{
		_swapChainAttachments = Attachments;
		_swapChainAttachmentCount = attachmentCount;
		_swapChainRenderPass = renderPass;

		_swapChainFrameBuffers.resize(_swapChainImageViews.size());

		for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
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
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &_swapChainFrameBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("FAILED TO CREATE FRAME BUFFER!");
			}
		}
	}

	/*void window::drawFrameExperimental(VkRenderPass renderPadss, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer,
		const uint16_t* indices, const uint16_t& indicesCount, const VkFormat& imageFormat)
	{
	}*/

	void window::updateSwapChain(const VkFormat& imageFormat, std::vector<VkSemaphore>& waitOn) {

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

		VkResult result = vkQueuePresentKHR(_presentQueue._queue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _procVAL->_frameBufferResized) {
			_procVAL->_frameBufferResized = false;
			recreateSwapChain(imageFormat);
		}
		else if (result != VK_SUCCESS) {
			printf("VAL: FAILED TO PRESENT SWAPCHAIN IMAGE\n");
			throw std::runtime_error("VAL: FAILED TO PRESENT SWAPCHAIN IMAGE");
		}
	}

	void window::waitForFences() {
		vkWaitForFences(_procVAL->_device, 1, &_presentQueue._fences[_procVAL->_currentFrame], VK_TRUE, UINT64_MAX);
	}

	VkFramebuffer& window::getSwapchainFramebuffer(const VkFormat& imageFormat) {
		//vkWaitForFences(_procVAL->_device, 1, &_presentQueue._fences[_procVAL->_currentFrame], VK_TRUE, UINT64_MAX);

		//uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_procVAL->_device, _swapChain, UINT64_MAX,
			_presentQueue._semaphores[_procVAL->_currentFrame], VK_NULL_HANDLE, &_currentSwapChainImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			vkDeviceWaitIdle(_procVAL->_device);
			recreateSwapChain(imageFormat);
			return _swapChainFrameBuffers[_currentSwapChainImageIndex];
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("VAL: Failed to acquire swap chain image!");
		}

		return _swapChainFrameBuffers[_currentSwapChainImageIndex];
	}

	// returns a swapchain frame buffer to use
	VkFramebuffer& window::beginDraw(const VkFormat& imageFormat) {
		vkResetFences(_procVAL->_device, 1, &_presentQueue._fences[_procVAL->_currentFrame]);
		VkFramebuffer& framebuffer = getSwapchainFramebuffer(imageFormat); // gets the swapchain framebuffer to be rendered to
		return framebuffer;
	}
}