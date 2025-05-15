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
#include <VAL/lib/system/queueManager.hpp>
#include <VAL/lib/system/windowProperties.hpp>
#include <vector>
#include <VAL/lib/ext/tiny_vector.hpp>


namespace val {
	class VAL_PROC; // forward declaration

	class window {
	public:
		window() = default;
		window(VAL_PROC& valProc) {
			_procVAL = &valProc;
		}
		window(GLFWwindow* windowHDL, VAL_PROC* valProc, VkColorSpaceKHR colorSpace) {
			if (!windowHDL) {
				printf("VAL: ERROR: Cannot create window, the GLFWwindow* handle is NULL! Ensure that glfwInit was called before the window's creation.");
				throw std::runtime_error("VAL: ERROR: Cannot create window, the GLFWwindow* handle is NULL! Ensure that glfwInit was called before the window's creation.");
			}
			_ownsGLFWwindow = false;
			_window = windowHDL;
			_procVAL = valProc;
			_colorSpace = colorSpace;
		}

		window(windowProperties& initProperties, const uint16_t width, const uint16_t height, const std::string& name, VAL_PROC* valProc, VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, GLFWmonitor* monitor = NULL) {
			glfwInit(); // (it's safe to call init more than once. Refer to: https://www.glfw.org/docs/3.3/intro_guide.html#intro_init_init)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
			
			initProperties.applyToGLFW();

			_window = glfwCreateWindow(width, height, name.c_str(), monitor, NULL);
			if (!_window) { printf("VAL: ERROR: Failed to initialize GLFW window, window is: %p \n", _window); }

			_procVAL = valProc;
			_colorSpace = colorSpace;
		}

		~window() {
			cleanup();
		}
	public:
		void setWindowHandleGLFW(GLFWwindow* windowHDL);
		GLFWwindow* getWindowHandleGLFW();

	public:

		void configure(GLFWwindow* windowHDL, VkColorSpaceKHR colorSpace);

		void display(const VkFormat& imgFormat, std::vector<VkSemaphore> waitOn);

		void createWindowSurface(VkInstance instance);

		void cleanup();

		void cleanupSwapChain();

		void createSwapChain(const VkFormat swapchainFormat);

		void recreateSwapChain(const VkFormat swapchainFormat);

		void createSwapChainImageViews(const VkFormat swapchainFormat);

		void createSwapChainFrameBuffers(const VkExtent2D& extent, VkImageView* Attachments, const uint16_t& attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice);

		void updateSwapChain(const VkFormat& imageFormat, std::vector<VkSemaphore>& waitOn);

		void waitForFences();

		VkFramebuffer& getSwapchainFramebuffer(const VkFormat& imageFormat); // gets the swapchain framebuffer for rendering

		//void createPresentQueue();

		VkFramebuffer& beginDraw(const VkFormat& imageFormat);

		VkFence getPresentFence();

		inline VkExtent2D getSize() {
			return {_swapChainExtent.width, _swapChainExtent.height};
		}

		inline bool shouldClose() {
			return glfwWindowShouldClose(_window);
		}
	public:

		uint32_t _currentSwapChainImageIndex = 0;

		VAL_PROC* _procVAL = NULL;

		GLFWwindow* _window = NULL;

		VkColorSpaceKHR _colorSpace;

		queueManager _presentQueue;

		VkSurfaceKHR _surface{};
		////////////////// SWAPCHAIN //////////////////

		VkSwapchainKHR _swapChain{};
		tiny_vector<VkImage, uint8_t> _swapChainImages;
		//VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent{};
		tiny_vector<VkImageView, uint8_t> _swapChainImageViews;
		tiny_vector<VkFramebuffer, uint8_t> _swapChainFrameBuffers;

		// this data is used to recreate the swap chain when it's out of date.
		VkImageView* _swapChainAttachments;
		uint16_t _swapChainAttachmentCount;
		VkRenderPass _swapChainRenderPass{};

		// Because windows can be created from an existing GLFW handle,
		// this is used to avoid the destruction of a window that it doesn't own.
		bool _ownsGLFWwindow = true;
	};
}

#endif // !VAL_WINDOW_HPP