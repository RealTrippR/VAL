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

#ifndef VAL_PUSH_DESCRIPTOR_HPP
#define VAL_PUSH_DESCRIPTOR_HPP

#include <VAL/lib/system/descType.hpp>
#include <stdio.h>
#include <stdint.h>

namespace val {
	class pushDescriptor {
	public:
		pushDescriptor() = default;
		// intializes a push descriptor, the valueCount is implicitly set to 1.
		pushDescriptor(uint16_t bindingIndex, DESC_TYPE type) {
			_valueCount = 1;
			_bindingIndex = bindingIndex;
			_type = type;
		}
		pushDescriptor(uint16_t valueCount, uint16_t bindingIndex, DESC_TYPE type) {
#ifndef NDEBUG
			if (valueCount == 0) {
				printf("VAL: WARNING: setting the valuCount of a pushDescriptor to 0 will invalidate it\n");
			}
#endif // !NDEBUG
			_valueCount = valueCount;
			_bindingIndex = bindingIndex;
			_type = type;
		}

	public:
		void setBindingIndex(const uint16_t index);
		uint16_t getBindingIndex();
		void setValueCount(const uint16_t count);
		uint16_t getValueCount();
		void setDescType(const DESC_TYPE dtype);
		DESC_TYPE getDescType();
	private:
		uint16_t _valueCount = 0u;
		uint16_t _bindingIndex = 0u;
		DESC_TYPE _type;
	};
}

#endif // !VAL_PUSH_DESCRIPTOR_HPP
