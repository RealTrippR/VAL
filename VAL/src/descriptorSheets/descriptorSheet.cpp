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

#include <VAL/lib/descriptorSheets/descriptorSheet.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <unordered_map>

namespace val
{
	VAL_RETURN_CODE DescriptorSheet::createDescriptorSetLayoutFromSheet(VkDevice device, VkDescriptorSetLayout* layout)
	{
		tiny_vector<VkDescriptorSetLayoutBinding> bindings(_elements.size());

		for (uint32_t i = 0; i < _elements.size(); ++i)
		{
			bindings[i] = _elements[i].toVkDescriptorSetLayoutBinding();
		}

		const VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0x0,
			.bindingCount = (uint32_t)bindings.size(),
			.pBindings = bindings.data()
		};

		const VkResult createRes = vkCreateDescriptorSetLayout(device, &layoutCreateInfo, NULL, layout);
		if (createRes != VK_SUCCESS)
		{
			dbg::printError("DescriptorSheet::createDescriptorSetLayoutFromSheet: vkCreateDescriptorSetLayout Failed, VkResult: %lu", createRes);
			return VAL_FAILURE;
		}

		return VAL_SUCCESS;
	}

	uint32_t DescriptorSheet::getDescriptorPoolSizes(std::vector<VkDescriptorPoolSize>* descPoolSizes) const
	{
		std::unordered_map<VkDescriptorType, uint32_t> poolSizesMap;
		for (uint32_t ei = 0; ei < _elements.size(); ++ei)
		{
			const DescriptorSheetElement& element = _elements[ei];
			poolSizesMap[element.getType()] += element.getArraySize();
		}

		uint32_t i = (uint32_t)descPoolSizes->size();
		descPoolSizes->resize(descPoolSizes->size() + poolSizesMap.size());
		for (const auto& poolSize : poolSizesMap)
		{
			(*descPoolSizes)[i] = { poolSize.first, poolSize.second };
			++i;
		}
		return (uint32_t)poolSizesMap.size();
	}

	VAL_RETURN_CODE DescriptorSheet::allocateAndWriteSets(VkDevice device, VkDescriptorSet* sets, VkDescriptorSetLayout* setLayouts, uint32_t setCount, VkDescriptorPool pool)
	{
		if (sets == NULL) {
			dbg::printError("DescriptorSheet::allocateAndWriteSets: DescriptorSheet @ %p: argument `sets` was NULL.", this);
			return VAL_FAILURE;
		}
		if (setLayouts == NULL) {
			dbg::printError("DescriptorSheet::allocateAndWriteSets: DescriptorSheet @ %p: argument `setLayouts` was NULL.", this);
			return VAL_FAILURE;
		}
		const VkDescriptorSetAllocateInfo setAllocInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.descriptorPool = pool,
			.descriptorSetCount = setCount,
			.pSetLayouts = setLayouts
		};
	
		if (vkAllocateDescriptorSets(device, &setAllocInfo, sets) != VK_SUCCESS)
		{
			dbg::printError("DescriptorSheet::allocateSetAndWriteSets: DescriptorSheet @ %p: failed to allocate descriptor sets, of which there are %lu.", this, setCount);
			return VAL_FAILURE;
		}

		for (uint32_t i = 0; i < setCount; ++i)
		{
			updateAndWriteDescriptors(device, sets[i]);
		}

		return VAL_SUCCESS;
	}

	void DescriptorSheet::updateAndWriteDescriptors(VkDevice device, VkDescriptorSet descriptorSet)
	{
		// update data with callbacks
		for (uint32_t i = 0; i < _elements.size(); ++i)
		{
			DescriptorSheetElement& element = _elements[i];
			ObjectDescriptorInfo& descriptorInfo = element._descriptorInfo;
			if (descriptorInfo.updateDataCallback == NULL) {
				dbg::printWarning("DescriptorSheet::updateDescriptors: For DescriptorSheet @ %p, the updateDataCallback of element #%lu is NULL", this, i);
				continue;
			}

			descriptorInfo.updateWithDataCallback();
			if (element._overrideImgLayout != IMAGE_LAYOUT::MaxEnum) {
				for (auto& imgInfo : element._descriptorInfo.imageInfos) {
					imgInfo.imageLayout = (VkImageLayout)element._overrideImgLayout;
				}
			}
		}

		_descriptorWrites.resize(_elements.size());
		for (uint32_t i = 0; i < _elements.size(); ++i)
		{
			_descriptorWrites[i] = _elements[i].toVkWriteDescriptorSet(descriptorSet);
			if (_descriptorWrites[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
				if (_descriptorWrites[i].pImageInfo == NULL || _descriptorWrites[i].pImageInfo[0].sampler == NULL) {
					dbg::printError("DescriptorSheet::updateAndWriteDescriptors: Descriptor #%u is of VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER but it's sampler is NULL.", i);
				}
			}
		}

#ifndef NDEBUG
		if (_descriptorWrites.data() == NULL || _descriptorWrites.size() == 0)
		{	
			dbg::printError("DescriptorSheet::updateDescriptors: Failed to update DescriptorSheet @ %p: no descriptors to update, element count is 0.", this);
			return;
		}
#endif
		vkUpdateDescriptorSets(device, _descriptorWrites.size(), _descriptorWrites.data(), 0, 0);
	}

	void DescriptorSheet::updateDescriptor(VkDevice device, const uint32_t index)
	{
#ifndef NDEBUG
		if (_descriptorWrites.data() == NULL || _descriptorWrites.size() == 0)
		{
			dbg::printError("DescriptorSheet::updateDescriptors: Failed to update DescriptorSheet @ %p: no descriptors to update, element count is 0.", this);
			return;
		}
		if (index >= _descriptorWrites.size())
		{
			dbg::printError("DescriptorSheet::updateDescriptors: Failed to update DescriptorSheet @ %p: index of %lu is out of bounds.", this, index);
			return;
		}
#endif // !NDEBUG
		vkUpdateDescriptorSets(device, 1, _descriptorWrites.data() + index, 0, 0);
	}

	void DescriptorSheet::addSheetElement(const DescriptorSheetElement& element)
	{
		_elements.push_back(element);

#ifndef NDEBUG
		for (const auto& e1 : _elements) {
			for (const auto& e2 : _elements) {
				if (&e1 != &e2 && e1.getBinding() == e2.getBinding()) {
					dbg::printError("DescriptorSheet::addSheetElement: Two elements must not share the same binding index.");
					throw std::logic_error("DescriptorSheet::addSheetElement: Two elements must not share the same binding index.");
				}
			}
		}
#endif // !NDEBUG
	}

	void DescriptorSheet::insertSheetElement(const uint32_t index, const DescriptorSheetElement& element)
	{
		if (index >= _elements.size()) {
			dbg::printWarning("DescriptorSheet::insertSheetElement: index %lu exceeds elements array of size %llu", index, _elements.size());
			return;
		}
		_elements.insert(_elements.begin() + index, element);
	}

	void DescriptorSheet::setSheetElement(const uint32_t index, const DescriptorSheetElement& element)
	{
		if (index >= _elements.size()) {
			dbg::printWarning("DescriptorSheet::setSheetElement: index %lu exceeds elements array of size %llu", index, _elements.size());
			return;
		}
		_elements[index] = element;
	}

	void DescriptorSheet::removeSheetElement(const uint32_t index)
	{
		if (index >= _elements.size()) {
			dbg::printWarning("DescriptorSheet::removeSheetElement: index %lu exceeds elements array of size %llu", index, _elements.size());
			return;
		}
		_elements.erase(_elements.begin() + index);
	}

	const tiny_vector<DescriptorSheetElement>& DescriptorSheet::getSheetElements() const
	{
		return _elements;
	}
}