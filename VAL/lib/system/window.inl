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


#ifndef VAL_WINDOW_INLINE
#define VAL_WINDOW_INLINE

#include <VAL/lib/system/window.hpp>

namespace val {

	inline uint32_t Window::getHeight() const {
		return _swapChainExtent.height;
	}
	
	inline uint32_t Window::getWidth() const {
		return _swapChainExtent.width;
	}

	inline VkExtent2D Window::getSize() const {
		return { _swapChainExtent.width, _swapChainExtent.height };
	}

	inline VkRect2D Window::getSizeAsRect2D() const {
		return { 0,0, _swapChainExtent.width, _swapChainExtent.height };
	}

	inline bool Window::shouldClose() const {
		return glfwWindowShouldClose(_window);
	}

	inline VkSemaphore Window::getPresentSemaphore() const {
#ifndef NDEBUG
		if (_presentQueue.getCommandBufferCount() < _procVAL->getFramesInFlight()) {
			dbg::printError("Semaphore count of present queue for Window @ %p should equal the framesInFlight of it's ValProc. Hint: Ensure that the window is created after proc.create() has been called.", this);
			throw std::runtime_error("Semaphore count of present queue for a Window should equal the framesInFlight of it's ValProc. Hint: Ensure that the window is created after proc.create() has been called.");
		}
#endif // !NDEBUG

		return _presentQueue.getSemaphore(_procVAL->getCurrentFrame());
	}

	inline VkSemaphore Window::getPresentSemaphore(const uint8_t frameidx) const {
		return _presentQueue.getSemaphore(frameidx);
	}

	inline const VkColorSpaceKHR Window::getColorSpace() const {
		return _colorSpace;
	}

	inline const Queue& Window::getPresentQueue() const {
		return _presentQueue;
	}


	inline Queue& Window::getPresentQueue() {
		return _presentQueue;
	}

	inline ValProc* Window::getValProc() const {
		return _procVAL;
	}

	inline VkSurfaceKHR Window::getVkSurface() const {
		return _surface;
	}

}

#endif