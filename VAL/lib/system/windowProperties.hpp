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


namespace val {
	
	// see :: https://www.glfw.org/docs/latest/window_guide.html
	enum class WN_BOOL_PROPERTY : uint32_t {
		RESIZABLE =				1,
		VISIBLE =				2,
		MAXIMIZED =				4,
		CENTER_CURSOR =			8,
		SCALE_TO_MONITOR =		16,
		SCALE_FRAMEBUFFER =		32,
		MOUSE_PASSTHROUGH =		64
	};

	#ifndef WINDOW_BOOL_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS
		#define WINDOW_BOOL_PROPERTIES_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(WN_BOOL_PROPERTY);
	#endif

	class windowProperties {
	protected:
		WN_BOOL_PROPERTY _boolSetMask{}; // the values to be set
		WN_BOOL_PROPERTY _boolValueMask{};
	public:

		inline void setProperty(WN_BOOL_PROPERTY property, bool value) {
			_boolSetMask |= property;
			if (value) {
				_boolValueMask |= WN_BOOL_PROPERTY(1 << uint32_t(property)); //set val
			}
			else {
				_boolValueMask &= ~WN_BOOL_PROPERTY(1 << uint32_t(property)); //clear val
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
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::RESIZABLE)) {
				glfwWindowHint(GLFW_RESIZABLE,
					bool(_boolValueMask & WN_BOOL_PROPERTY::RESIZABLE));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::VISIBLE)) {
				glfwWindowHint(GLFW_VISIBLE,
					bool(_boolValueMask & WN_BOOL_PROPERTY::VISIBLE));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::MAXIMIZED)) {
				glfwWindowHint(GLFW_MAXIMIZED,
					bool(_boolValueMask & WN_BOOL_PROPERTY::MAXIMIZED));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::CENTER_CURSOR)) {
				glfwWindowHint(GLFW_CENTER_CURSOR,
					bool(_boolValueMask & WN_BOOL_PROPERTY::CENTER_CURSOR));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::SCALE_TO_MONITOR)) {
				glfwWindowHint(GLFW_SCALE_TO_MONITOR,
					bool(_boolValueMask & WN_BOOL_PROPERTY::SCALE_TO_MONITOR));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::SCALE_FRAMEBUFFER)) {
				glfwWindowHint(GLFW_SCALE_FRAMEBUFFER,
					bool(_boolValueMask & WN_BOOL_PROPERTY::SCALE_FRAMEBUFFER));
			}
			if (bool(_boolSetMask & WN_BOOL_PROPERTY::MOUSE_PASSTHROUGH)) {
				glfwWindowHint(GLFW_MOUSE_PASSTHROUGH,
					bool(_boolValueMask & WN_BOOL_PROPERTY::MOUSE_PASSTHROUGH));
			}
		}
	};
}

#endif // !VAL_WINDOW_PROPERTIES_HPP