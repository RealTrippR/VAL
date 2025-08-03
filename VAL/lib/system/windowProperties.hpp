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


#ifndef VAL_WINDOW_PROPERTIES_HPP
#define VAL_WINDOW_PROPERTIES_HPP

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <VAL/lib/classEnumBitOps.hpp>
#include <type_traits>

namespace val {
	
	// see :: https://www.glfw.org/docs/latest/window_guide.html
	enum WN_BOOL_PROPERTY : uint32_t {
		Resizable =				1,
		Visible =				2,
		Maximized =				4,
		CenterCursor =			8,
		ScaleToMonitor =		16,
		ScaleFramebuffer =		32,
		MousePassthrough =		64
	};

	enum class WN_MODE : uint8_t {
		Windowed,
		BorderlessWindowed,
		Fullscreen,
		WindowedFullscreen
	};
	#ifndef WINDOW_BOOL_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS
	#define WINDOW_BOOL_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(WN_BOOL_PROPERTY);
	#endif

	class WindowProperties {
	protected:
		WN_BOOL_PROPERTY _boolSetMask{}; // a mask which holds which values to be set
		WN_BOOL_PROPERTY _boolValueMask{}; // the actual values
	public:

		inline void setProperty(WN_BOOL_PROPERTY property, bool value) {
			_boolSetMask |= property;

			if (value) {
				WN_BOOL_PROPERTY flag = static_cast<WN_BOOL_PROPERTY>(1 << uint32_t(property));
				_boolValueMask |= flag; //set val
			}
			else {
				WN_BOOL_PROPERTY flag = static_cast<WN_BOOL_PROPERTY>(1 << uint32_t(property));
				flag = ~flag;
				_boolValueMask &= flag;
			}
		}

		inline bool getProperty(WN_BOOL_PROPERTY property) const {
		#ifndef NDEBUG
			if (bool(_boolSetMask & property) == 0) {
				printf("VAL: WARNING: attempted to get window property (enum val: %d) that has not been set. This will result in undefined behavior. _boolSetMask: ", property);
				for (uint32_t i = 0; i < sizeof(uint32_t) * 8; ++i) {
					// https://www.geeksforgeeks.org/extract-bits-in-c/
					uint32_t mask = 1 << i;
					uint32_t extracted_bit = (uint32_t(_boolSetMask) & mask) >> i;
					printf("%d", extracted_bit);
				}
				printf("\n");
			}
		#endif // !NDEBUG

			return bool(_boolValueMask & property);
		}

		inline WN_BOOL_PROPERTY getBoolSetMask() const {
			return _boolSetMask;
		}

		inline WN_BOOL_PROPERTY getBoolValueMask() const {
			return _boolValueMask;
		}

		inline void applyToGLFW() {
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::Resizable)) {
				glfwWindowHint(GLFW_RESIZABLE,
					bool(_boolValueMask & WN_BOOL_PROPERTY::Resizable));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::Visible)) {
				glfwWindowHint(GLFW_VISIBLE,
					bool(_boolValueMask & WN_BOOL_PROPERTY::Visible));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::Maximized)) {
				glfwWindowHint(GLFW_MAXIMIZED,
					bool(_boolValueMask & WN_BOOL_PROPERTY::Maximized));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::CenterCursor)) {
				glfwWindowHint(GLFW_CENTER_CURSOR,
					bool(_boolValueMask & WN_BOOL_PROPERTY::CenterCursor));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::ScaleToMonitor)) {
				glfwWindowHint(GLFW_SCALE_TO_MONITOR,
					bool(_boolValueMask & WN_BOOL_PROPERTY::ScaleToMonitor));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::ScaleFramebuffer)) {
				glfwWindowHint(GLFW_SCALE_FRAMEBUFFER,
					bool(_boolValueMask & WN_BOOL_PROPERTY::ScaleFramebuffer));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::MousePassthrough)) {
				glfwWindowHint(GLFW_MOUSE_PASSTHROUGH,
					bool(_boolValueMask & WN_BOOL_PROPERTY::MousePassthrough));
			}
		}
	};
}

#endif // !VAL_WINDOW_PROPERTIES_HPP