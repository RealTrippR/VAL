#ifndef VAL_WINDOW_HPP
#define VAL_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/queueManager.hpp>
#include <vector>


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
			_window = windowHDL;
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
	public:

		uint32_t _currentSwapChainImageIndex = 0;

		VAL_PROC* _procVAL = NULL;

		GLFWwindow* _window = NULL;

		VkColorSpaceKHR _colorSpace;

		queueManager _presentQueue;
		//VkQueue _presentQueue;

		VkSurfaceKHR _surface{};
		////////////////// SWAPCHAIN //////////////////

		VkSwapchainKHR _swapChain{};
		std::vector<VkImage> _swapChainImages;
		//VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent{};
		std::vector<VkImageView> _swapChainImageViews;
		std::vector<VkFramebuffer> _swapChainFrameBuffers;

		// this data is used to recreate the swap chain when it's out of date.
		VkImageView* _swapChainAttachments;
		uint16_t _swapChainAttachmentCount;
		VkRenderPass _swapChainRenderPass{};
	};
}

#endif // !VAL_WINDOW_HPP