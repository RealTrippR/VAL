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

#ifndef VAL_WINDOW_HPP
#define VAL_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/windowProperties.hpp>
#include <VAL/lib/system/windowCursor.hpp>
#include <vector> 
#include <VAL/lib/ext/tiny_vector.hpp>


namespace val {
	class ValProc; // forward declaration

	class Window {
	public:

		Window() = default;

		Window(ValProc& valProc) 
			: _presentQueue(NULL)
		{
			_procVAL = &valProc;
		}

		Window(GLFWwindow* windowHDL, ValProc& valProc, VkColorSpaceKHR colorSpace) 
			: _presentQueue(NULL)
		{
			if (!windowHDL) {
				printf("VAL: ERROR: Cannot create window, the GLFWwindow* handle is NULL! Ensure that glfwInit was called before the window's creation.");
				throw std::runtime_error("VAL: ERROR: Cannot create window, the GLFWwindow* handle is NULL! Ensure that glfwInit was called before the window's creation.");
			}
			_ownsGLFWwindow = false;
			_window = windowHDL;
			_procVAL = &valProc;
			_colorSpace = colorSpace;
		}

		Window(ValProc& proc, WindowProperties& initProperties, const uint16_t width, const uint16_t height, const char* title, VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, GLFWmonitor* monitor = NULL)
			: _presentQueue(NULL)
		{
			prep(proc, initProperties, width, height, title, colorSpace, monitor);
		}

		~Window() {
			cleanup();
		}
	public:
		void setWindowHandleGLFW(GLFWwindow* windowHDL);

		GLFWwindow* getWindowHandleGLFW();

	public:

		VAL_RETURN_CODE create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass);

		VAL_RETURN_CODE create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass, const tiny_vector<VkImageView>& attachments);

		VAL_RETURN_CODE create(const VkFormat swapchainFormat, VkCommandPool cmdPool, VkRenderPass renderPass, const VkImageView* attachments, const uint32_t attachmentCount);

		void prep(ValProc& proc, WindowProperties& initProperties, const uint16_t width, const uint16_t height, const char* title, const VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, GLFWmonitor* monitor = NULL);

		void setTitle(const char* title);

		const char* getTitle();

		void resize(const uint16_t width, const uint16_t height);
		// void setIconFromMemory(const void* mem, const uint32_t memSize);
		// 
		VAL_RETURN_CODE setIcon(const fs::path& filepath);

		void setCursor(const Cursor& cursor);

		void setWindowMode(const WN_MODE windowMode);

		void configure(GLFWwindow* windowHDL, VkColorSpaceKHR colorSpace);

		void display(const VkFormat imgFormat, VkSemaphore waitSemaphore);

		void display(const VkFormat imgFormat, VkSemaphore* waitSemaphores, uint32_t waitSemCount);

		void destroy(); // same as cleanup()

		void cleanup();

		/// cleanupSwapChain() - Destroys the swapchain and it's associated images and framebuffers.
		/// @brief 
		void cleanupSwapChain();

		VAL_RETURN_CODE createSwapChain(const VkFormat swapchainFormat);

		void recreateSwapChain(const VkFormat swapchainFormat);

		void updateSwapChain(const VkFormat& imageFormat, VkSemaphore* waitSemaphores, uint32_t waitSemaphoreCount);

		VkImageView getSwapchainImageView() const;

		VkImage getSwapchainImage() const;

		VkFramebuffer& getSwapchainFramebuffer(const VkFormat& imageFormat, VkSemaphore imgAvailableSemaphore); // gets the swapchain framebuffer for rendering

		inline uint8_t getSwapchainImageCount() const;

		inline uint32_t getHeight() const;

		inline uint32_t getWidth() const;
		
		inline VkExtent2D getSize() const;

		inline VkRect2D getSizeAsRect2D() const;

		inline bool shouldClose() const;

		inline const VkColorSpaceKHR getColorSpace() const;

		inline const Queue& getPresentQueue() const;

		inline Queue& getPresentQueue();

		inline ValProc* getValProc() const;

		inline VkSurfaceKHR getVkSurface() const;

		inline static void pollEvents() {
			glfwPollEvents();
		}

	protected:
		VAL_RETURN_CODE createSwapChainFrameBuffers(VkDevice device, VkRenderPass renderPass);

		VAL_RETURN_CODE createSwapChainFrameBuffers(const VkImageView* Attachments, const uint32_t attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice);

		void createPresentQueue(VkCommandPool cmdPool);

		void createPresentFence(VkDevice device);

		void createWindowSurface(VkInstance instance);

		void createSwapChainImageViews(const VkFormat swapchainFormat);

	protected:
		friend ValProc;

		//tiny_vector<VkSemaphore> semaphores;

		ValProc* _procVAL = NULL;

		GLFWwindow* _window = NULL;

		Queue _presentQueue;

		VkSurfaceKHR _surface{};
		////////////////// SWAPCHAIN //////////////////

		VkSwapchainKHR _swapChain{};
		VkExtent2D _swapChainExtent{};

		VkImage* _swapChainImages = VK_NULL_HANDLE;
		VkImageView* _swapChainImageViews = VK_NULL_HANDLE;
		VkFramebuffer* _swapChainFramebuffers = VK_NULL_HANDLE;

		//VkFence _presentFence = VK_NULL_HANDLE;

		// this data is used to recreate the swap chain when it's out of date.
		const VkImageView* _swapChainAttachments = NULL;
		VkRenderPass _swapChainRenderPass{};

		VkColorSpaceKHR _colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		uint32_t _currentSwapChainImageIndex = 0;

		uint16_t _swapChainAttachmentCount = 0u;

		uint8_t _swapChainImageCount = 0u;

		// Because windows can be created from an existing GLFW handle,
		// this is used to avoid the destruction of a window that it doesn't own.
		bool _ownsGLFWwindow = true;

		bool _frameBufferResized = false;

		WN_MODE _windowMode = WN_MODE::Windowed;
	};
}

#endif // !VAL_WINDOW_HPP