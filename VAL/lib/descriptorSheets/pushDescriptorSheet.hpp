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

#ifndef VAL_PUSH_DESCRIPTOR_SHEET_HPP
#define VAL_PUSH_DESCRIPTOR_SHEET_HPP

#include <vulkan/vulkan_core.h>
#include <VAL/lib/VALreturnCode.h>
#include <VAL/lib/ext/tiny_vector.hpp>
#include <VAL/lib/descriptorSheets/pushDescriptorSheetElement.hpp>

namespace val
{
	class PushDescriptorSheet
	{
	public:
		PushDescriptorSheet() = default;

		PushDescriptorSheet(const tiny_vector<PushDescriptorSheetElement>& elements) {
			setElements(elements);
		}
	public:
		VAL_RETURN_CODE create(VkDevice device);

		void destroy(VkDevice device);

		void setElements(const tiny_vector<PushDescriptorSheetElement>& elements);

		const tiny_vector<PushDescriptorSheetElement>& getElements() const;

		inline VkDescriptorSetLayout getVkDescriptorLayout() const { return _layout; }

	protected:
		VkDescriptorSetLayout _layout;
		tiny_vector<PushDescriptorSheetElement> _elements;
	};
}

#endif // !VAL_PUSH_DESCRIPTOR_SHEET_HPP