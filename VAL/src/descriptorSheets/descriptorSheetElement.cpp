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

#include <VAL/lib/descriptorSheets/descriptorSheetElement.hpp>

namespace val
{
	VkDescriptorType DescriptorSheetElement::getType() const
	{
		return _descriptorInfo.type;
	}

	uint32_t DescriptorSheetElement::getBinding() const
	{
		return _binding;
	}

	uint32_t DescriptorSheetElement::getArraySize() const
	{
		return _descriptorInfo.arrCount;
	}

	VkPipelineStageFlags DescriptorSheetElement::getStageFlags() const
	{
		return (VkPipelineStageFlags)_shaderStageFlags;
	}

	const ObjectDescriptorInfo& DescriptorSheetElement::getDescriptorInfo()
	{
		return _descriptorInfo;
	}

	VkWriteDescriptorSet DescriptorSheetElement::toVkWriteDescriptorSet(VkDescriptorSet descriptorSet) const
	{
		VkWriteDescriptorSet write;
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pImageInfo = _descriptorInfo.imageInfos.data();
		write.pBufferInfo = _descriptorInfo.bufferInfos.data();
		write.pTexelBufferView = _descriptorInfo.texelBufferViews.data();
		write.dstBinding = _binding;
		write.descriptorCount = _descriptorInfo.arrCount;
		write.dstArrayElement = 0; // start writing at 0, write descriptorCount elements.
		write.pNext = _descriptorInfo.pNext;
		write.descriptorType = _descriptorInfo.type;
		write.dstSet = descriptorSet;

		return write;
	}

	VkDescriptorSetLayoutBinding DescriptorSheetElement::toVkDescriptorSetLayoutBinding() const
	{
		VkDescriptorSetLayoutBinding layoutBinding = {
			.binding = _binding,
			.descriptorType = _descriptorInfo.type,
			.descriptorCount = _descriptorInfo.arrCount,
			.stageFlags = _shaderStageFlags,
			.pImmutableSamplers = VK_NULL_HANDLE
		};

		return layoutBinding;
	}
}