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

#ifndef VAL_DESCRIPTOR_SHEET_ELEMENT_HPP
#define VAL_DESCRIPTOR_SHEET_ELEMENT_HPP

#include <vulkan/vulkan_core.h>
#include <VAL/lib/graphics/shader.hpp>
namespace val
{
	class DescriptorSheet; // forward declaration

	class DescriptorSheetElement
	{
	public:
		DescriptorSheetElement() = default;
		DescriptorSheetElement(const uint32_t binding) {
			_binding = binding;
		}

	public:
		VkDescriptorType getType() const; 

		uint32_t getBinding() const;
	
		uint32_t getArraySize() const;

		VkPipelineStageFlags getStageFlags() const;

		const ObjectDescriptorInfo& getDescriptorInfo();

		inline void* getValObject() const
		{
			return _descriptorInfo.valObject;
		}
	public:
		
		VkWriteDescriptorSet toVkWriteDescriptorSet(VkDescriptorSet descriptorSet) const;

		VkDescriptorSetLayoutBinding toVkDescriptorSetLayoutBinding() const;

	protected:
		friend DescriptorSheet;
		uint32_t _binding=0u;
		VkShaderStageFlags _shaderStageFlags = 0x0;
		ObjectDescriptorInfo _descriptorInfo;
	};
}

#endif // !VAL_DESCRIPTOR_SHEET_ELEMENT_HPP
