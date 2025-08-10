#include <VAL/lib/descriptorSheets/pushDescriptorSheet.hpp>

namespace val
{
	VAL_RETURN_CODE PushDescriptorSheet::create(VkDevice device)
	{
		tiny_vector<VkDescriptorSetLayoutBinding> bindings(getElements().size());
		for (uint32_t i = 0; i < bindings.size(); ++i)
		{
			VkDescriptorSetLayoutBinding& binding = bindings[i];
			memcpy(&binding, &getElements()[i], sizeof(PushDescriptorSheetElement));
			binding.pImmutableSamplers = NULL;			
		}

		VkDescriptorSetLayoutCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT;
		createInfo.bindingCount = bindings.size();
		createInfo.pBindings = bindings.data();
		if (vkCreateDescriptorSetLayout(device, &createInfo, NULL, &_layout) != VK_SUCCESS)
			return VAL_FAILURE;
		return VAL_SUCCESS;
	}

	void PushDescriptorSheet::destroy(VkDevice device)
	{
		vkDestroyDescriptorSetLayout(device, _layout, NULL);
	}

	void PushDescriptorSheet::setElements(const tiny_vector<PushDescriptorSheetElement>& elements)
	{
		_elements = elements;
	}

	const tiny_vector<PushDescriptorSheetElement>& PushDescriptorSheet::getElements() const
	{
		return _elements;
	}
}