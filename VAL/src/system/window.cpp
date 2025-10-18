#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void Window::setWindowHandleGLFW(GLFWwindow* window) {
		_window = window;
	}

	GLFWwindow* Window::getWindowHandleGLFW() {
		return _window;
	}

	void Window::configure(GLFWwindow* windowHDL, VkColorSpaceKHR colorSpace) 
	{
		_window = windowHDL;
		_colorSpace = colorSpace;
	}

	VAL_RETURN_CODE Window::create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass)
	{
		createWindowSurface(_procVAL->_instance);
		createPresentQueue(cmdPool);
		createPresentFence(_procVAL->getVkLogicalDevice());
		if (createSwapChain(swapchainFormat) != VAL_SUCCESS)
			return VAL_FAILURE;
		if (createSwapChainFrameBuffers(*_procVAL, renderPass) != VAL_SUCCESS)
			return VAL_FAILURE;

		setWindowMode(_windowMode); // Refresh window mode
		return VAL_SUCCESS;
	}

	
	VAL_RETURN_CODE Window::create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass, const tiny_vector<VkImageView>& attachments)
	{
		createWindowSurface(_procVAL->_instance);
		createPresentQueue(cmdPool);
		createPresentFence(_procVAL->getVkLogicalDevice());
		if (createSwapChain(swapchainFormat) != VAL_SUCCESS)
			return VAL_FAILURE;
		if (createSwapChainFrameBuffers(attachments.data(), attachments.size(), renderPass, _procVAL->getVkLogicalDevice()) != VAL_SUCCESS)
			return VAL_FAILURE;

		setWindowMode(_windowMode); // Refresh window mode
		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE Window::create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass, const VkImageView* attachments, const uint32_t attachmentCount)
	{
		createWindowSurface(_procVAL->_instance);
		createPresentQueue(cmdPool);
		createPresentFence(_procVAL->getVkLogicalDevice());
		if (createSwapChain(swapchainFormat) != VK_SUCCESS)
			return VAL_FAILURE;
		if (createSwapChainFrameBuffers(attachments, attachmentCount, renderPass, _procVAL->getVkLogicalDevice()) != VAL_SUCCESS)
			return VAL_FAILURE;

		setWindowMode(_windowMode); // Refresh window mode
		return VAL_SUCCESS;
	}

	void Window::prep(ValProc& proc, WindowProperties& initProperties, const uint16_t width, const uint16_t height, const char* title, const VkColorSpaceKHR colorSpace, GLFWmonitor* monitor)
	{
#ifndef NDEBUG
		if (proc._instance == NULL) {
			dbg::printWarning("Window::prep: for Window @ %p: Calling Window.prep before the VkInstance of the given proc has been created may result in undefined behavior.", this);
		}
#endif // !NDEBUG

		_procVAL = &proc;

		glfwInit(); // (it's safe to call init more than once. Refer to: https://www.glfw.org/docs/3.3/intro_guide.html#intro_init_init)

		initProperties.applyToGLFW();

		_window = glfwCreateWindow(width, height, title, monitor, NULL);
		glfwPollEvents();
		glfwWaitEvents();

		if (!_window) {
			dbg::printWarning("Window::prep: For Window @ %p: glfwCreateWindow failed.", this);
		}

		_procVAL = &proc;
		_colorSpace = colorSpace;
		_swapChainExtent = { .width = width, .height = height };
		_swapChainAttachmentCount = 0u;
	}

	void Window::setTitle(const char* title)
	{
		glfwSetWindowTitle(_window, title);
	}

	const char* Window::getTitle()
	{
		return glfwGetWindowTitle(_window);
	}

	void Window::resize(const uint16_t width, const uint16_t height)
	{
#ifndef NDEBUG
		if (_windowMode == WN_MODE::Fullscreen || _windowMode == WN_MODE::WindowedFullscreen) {
			dbg::printWarning("Window::resize: For Window @ %p:  resize should never be called on a Window which has a mode of WN_MODE::Fullscreen or WN_MODE::BorderlessWindowed.");
		}
#endif // !NDEBUG

		if (!_window)
			return;
		
		glfwSetWindowSize(_window, width, height);
	}

	VAL_RETURN_CODE Window::setIcon(const fs::path& filepath)
	{

		GLFWimage images[1];
		images[0].pixels = stbi_load(filepath.string().c_str(), &images[0].width, &images[0].height, 0, 4); // Load as RGBA
		
		if (images[0].pixels) {
			glfwSetWindowIcon(_window, 1, images);
			stbi_image_free(images[0].pixels);
			return VAL_SUCCESS;
		}
		else {
			return VAL_FAILURE;
		}

	}

	void Window::setCursor(const Cursor& cursor)
	{
		glfwSetCursor(_window, cursor);
	}

	void Window::setWindowMode(const WN_MODE windowMode)
	{
		_windowMode = windowMode;

		if (_window==nullptr) {
			return;
		}

		// EVERYTHING BELOW THIS LINE WILL DEREFERENCE _window
		// ==================================================================

		if (windowMode == WN_MODE::Fullscreen) {
			glfwSetWindowAttrib(_window, GLFW_DECORATED, false);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(
				_window, monitor,
				0, 0,						 // x, y position (ignored for fullscreen)
				mode->width, mode->height,	// width and height
				GLFW_DONT_CARE				// `GLFW_DONT_CARE` to keep current refresh rate
			);      
		}
		else if (windowMode == WN_MODE::WindowedFullscreen)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowAttrib(_window, GLFW_DECORATED, true);

			glfwSetWindowMonitor(
				_window, NULL,
				0, 0,				
				mode->width, mode->height,
				GLFW_DONT_CARE
			);

			glfwSetWindowSize(_window, mode->width, mode->height);

			glfwMaximizeWindow(_window);

			int width, height;

			glfwGetWindowSize(_window, &width, &height);
			_swapChainExtent = { .width = (uint32_t)width, .height = (uint32_t)height };
			_frameBufferResized = true;
		}
		
		else if (windowMode == WN_MODE::Windowed) {
			glfwSetWindowAttrib(_window, GLFW_DECORATED, true);

			int x=0, y=0;
			glfwGetWindowPos(_window,&x,&y);
			glfwSetWindowMonitor(_window, NULL,
				x,y,
				_swapChainExtent.width, _swapChainExtent.height,
				0);
		}
		else if (windowMode == WN_MODE::BorderlessWindowed) {
			glfwSetWindowAttrib(_window, GLFW_DECORATED, false); // GLFW_DECORATED is like a border flag

			int x = 0, y = 0;
			glfwGetWindowPos(_window, &x, &y);
			glfwSetWindowMonitor(_window, NULL,
				x, y,
				_swapChainExtent.width, _swapChainExtent.height,
				0);
		}
		return;
	}

	void Window::display(const VkFormat imgFormat, VkSemaphore waitSemaphore)
	{
		updateSwapChain(imgFormat, &waitSemaphore,1);
	}

	void Window::display(const VkFormat imageFormat, VkSemaphore* waitSemaphores, uint32_t waitSemaphoreCount) {
		updateSwapChain(imageFormat, waitSemaphores, waitSemaphoreCount);
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


			_surface = NULL;

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


	void Window::createPresentQueue(VkCommandPool cmdPool)
	{
		_presentQueue.create(*_procVAL, cmdPool, QUEUE_FLAGS::Graphics);
	}


	VAL_RETURN_CODE Window::createSwapChain(const VkFormat swapchainFormat)
	{
#ifndef NDEBUG
		if (_swapChain != VK_NULL_HANDLE)
		{
			dbg::printWarning("createSwapChain was called on window %h that already has an initialized swapchain, calling this function more than once may lead to undefined behavior or program crashes.", this);
			return VAL_FAILURE;
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
			return VAL_FAILURE;
		}

		_swapChainImageCount = swapChainImageCount;
		_swapChainExtent = extent;

		createSwapChainImageViews(swapchainFormat);

		return VAL_SUCCESS;
	}




	void Window::recreateSwapChain(const VkFormat swapchainFormat) {
		vkDeviceWaitIdle(_procVAL->_device);

		cleanupSwapChain();

		int width = 0, height = 0;
		glfwGetFramebufferSize(_window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(_window, &width, &height);
			glfwWaitEvents();
		}
		_swapChainExtent.width = width;
		_swapChainExtent.height = height;

		createSwapChain(swapchainFormat);
		createSwapChainFrameBuffers(_swapChainAttachments, _swapChainAttachmentCount, _swapChainRenderPass, _procVAL->_device);
	}

	VAL_RETURN_CODE Window::createSwapChainFrameBuffers(VkDevice device, VkRenderPass renderPass)
	{
		return createSwapChainFrameBuffers({}, 0u, renderPass, device);
	}


	VAL_RETURN_CODE Window::createSwapChainFrameBuffers(const VkImageView* Attachments, const uint32_t attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice)
	{
		_swapChainAttachments = Attachments;
		_swapChainAttachmentCount = attachmentCount;
		_swapChainRenderPass = renderPass;

#ifndef NDEBUG
		if (renderPass == VK_NULL_HANDLE) {
			dbg::printError("Window::createSwapChainFrameBuffers: For Window @ %p, renderPass is NULL.");
		}
#endif // !NDEBUG

		

		if (_swapChainImageCount > 0) {
			// allocate swap chain image views
			VkFramebuffer* tmp = (VkFramebuffer*)realloc(_swapChainFramebuffers, sizeof(VkFramebuffer) * _swapChainImageCount);
			if (tmp == NULL) {
				if (_swapChainFramebuffers) {
					free(_swapChainFramebuffers);
					_swapChainFramebuffers = NULL;
				}
				dbg::printError("Failed to allocate frambuffers of Window @ %p: out of system memory.", this);
				return VAL_FAILURE;
			}
			else {
				_swapChainFramebuffers = tmp;
			}

			for (size_t i = 0; i < _swapChainImageCount; i++) {
				//std::vector<VkImageView> attachmentsV = { _swapChainImageViews[i] };
				std::vector<VkImageView> attachmentsV;
				// add additional attachments
				attachmentsV.push_back(_swapChainImageViews[i]);

				attachmentsV.resize(attachmentCount + attachmentsV.size());
				for (uint16_t i = 0; i < attachmentCount; ++i) {
					attachmentsV[i+1] = Attachments[i];
				}

				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = (uint32_t)attachmentsV.size();
				framebufferInfo.pAttachments = attachmentsV.data();
				framebufferInfo.width = _swapChainExtent.width;
				framebufferInfo.height = _swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
					if (renderPass == VK_NULL_HANDLE) {
						dbg::printError("Window::createSwapChainFrameBuffers: For Window @ %p: Failed to create frame buffer %u",i);
						return VAL_FAILURE;
					}
				}
			}
		}
		return VAL_SUCCESS;
	}

	void Window::updateSwapChain(const VkFormat& imageFormat, VkSemaphore* waitSemaphores, uint32_t semaphoreCount) {

		const auto& currentFrame = _procVAL->_currentFrame;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		// semaphores to be waited on
		presentInfo.waitSemaphoreCount = semaphoreCount;
		presentInfo.pWaitSemaphores = waitSemaphores;

		VkSwapchainKHR swapChains[] = { _swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &_currentSwapChainImageIndex;

		VkResult result = vkQueuePresentKHR(_presentQueue.getVkQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameBufferResized) {
			_frameBufferResized = false;
			recreateSwapChain(imageFormat);
		}
		else if (result != VK_SUCCESS) {
			dbg::printError("Window::updateSwapChain::Failed to update swapchain of Window @ %p: result is %lu", this, result);
		}
	}

	VkImageView Window::getSwapchainImageView() const
	{
		return _swapChainImageViews[_currentSwapChainImageIndex];
	}

	VkImage Window::getSwapchainImage() const
	{
		return _swapChainImages[_currentSwapChainImageIndex];
	}

	VkFramebuffer& Window::getSwapchainFramebuffer(const VkFormat& imageFormat, VkSemaphore sem) {
		//vkWaitForFences(_procVAL->_device, 1, &_presentQueue._fences[_procVAL->_currentFrame], VK_TRUE, UINT64_MAX);

		//uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_procVAL->_device, _swapChain, UINT64_MAX,
			sem, VK_NULL_HANDLE, &_currentSwapChainImageIndex);

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

	void Window::createPresentFence(VkDevice device)
	{
		const VkFenceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};
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
			return;
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