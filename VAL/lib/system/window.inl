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
	inline VkFence& Window::getPresentFence() {
		return _presentQueue._fences[_procVAL->_currentFrame];
	}

	inline QueueManager& Window::getPresentQueue() {
		return _presentQueue;
	}

	inline uint32_t Window::getHeight() {
		return _swapChainExtent.height;
	}
	
	inline uint32_t Window::getWidth() {
		return _swapChainExtent.width;
	}

	inline VkExtent2D Window::getSize() {
		return { _swapChainExtent.width, _swapChainExtent.height };
	}

	inline VkRect2D Window::getSizeAsRect2D() {
		return { 0,0, _swapChainExtent.width, _swapChainExtent.height };
	}

	inline bool Window::shouldClose() {
		return glfwWindowShouldClose(_window);
	}

	inline VkSemaphore Window::getCurrentSemaphore() {
		return _presentQueue.getSemaphore(_procVAL->getCurrentFrame());
	}

	inline VkSemaphore Window::getSemaphore(const uint8_t frameidx) {
		return _presentQueue.getSemaphore(frameidx);
	}

	inline const VkColorSpaceKHR Window::getColorSpace() {
		return _colorSpace;
	}
}

#endif