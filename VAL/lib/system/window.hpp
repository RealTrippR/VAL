#ifndef VAL_WINDOW_HPP
#define VAL_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <vector>


namespace val {
	class VAL_PROC; // forward declaration

	class window {
	public:
		window() = default;
		window(GLFWwindow* windowHDL, VAL_PROC* valProc, VkColorSpaceKHR colorSpace) {
			_window = windowHDL;
			_procVAL = valProc;
			_colorSpace = colorSpace;
			//fmlProc->_windowFML = this;
		}
		// void init(const uint32_t& size_x, const uint32_t& size_y, const char* windowTitle, GLFWmonitor* monitor = NULL);
		// void windowSetMonitor();
		// void close();
		// void minimize();
		// void resize(const uint32_t &size_x, const uint32_t &size_y);
		// void reposition(const uint32_t &pos_x, const uint32_t &pos_y);
		void setWindowHandleGLFW(GLFWwindow* windowHDL);
		GLFWwindow* getWindowHandleGLFW();

	public:

		void createWindowSurface(VkInstance instance);

		void createSyncObjects();

		void cleanupSwapChain();

		void createSwapChain(const VkFormat swapchainFormat);

		void recreateSwapChain(const VkFormat swapchainFormat);

		void createSwapChainImageViews(const VkFormat swapchainFormat);

		void createSwapChainFrameBuffers(const VkExtent2D& extent, VkImageView* Attachments, const uint16_t& attachmentCount, VkRenderPass renderPass, VkDevice logicalDevice);

		/*void drawFrameExperimental(VkRenderPass renderPass, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer,
			const uint16_t* indices, const uint64_t& indicesCount, const VkFormat& imageFormat);*/

		void updateSwapChain(const VkFormat& imageFormat);

		void waitForFences();

		VkFramebuffer& getSwapchainFramebuffer(const VkFormat& imageFormat); // gets the swapchain framebuffer for rendering

		//void createPresentQueue();

		VkFramebuffer& beginDraw(const VkFormat& imageFormat);

	public:

		uint32_t _currentSwapChainImageIndex = 0;

		VAL_PROC* _procVAL = NULL;

		GLFWwindow* _window = NULL;

		VkColorSpaceKHR _colorSpace;

		VkQueue _presentQueue;

		VkSurfaceKHR _surface;
		////////////////// SWAPCHAIN //////////////////

		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		//VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		std::vector<VkImageView> _swapChainImageViews;
		std::vector<VkFramebuffer> _swapChainFrameBuffers;

		// this data is used to recreate the swap chain when it's out of date.
		VkImageView* _swapChainAttachments;
		uint16_t _swapChainAttachmentCount;
		VkRenderPass _swapChainRenderPass;





		////////////////// SYNC OBJECTS //////////////////

		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;

		std::vector<VkFence> _inFlightFences;

		///////////////////////////////////////////////
	};
}

#endif // !VAL_WINDOW_HPP