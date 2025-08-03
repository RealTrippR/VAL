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

#ifndef VAL_DESCRIPTOR_SHEET_HPP
#define VAL_DESCRIPTOR_SHEET_HPP

#include <VAL/lib/descriptorSheets/descriptorSheetElement.hpp>
#include <VAL/lib/VALreturnCode.h>
#include <VAL/lib/ext/tiny_vector.hpp>
#include <VAL/lib/system/shaderStageEnum.hpp>
#include <initializer_list>
#include <tuple>

namespace val
{
	static const ObjectDescriptorInfo DescriptorCombinedSampler =
	{
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.arrCount = 1
	};

	struct DescriptorSheetElementBuilder {
		uint32_t binding;
		ObjectDescriptorInfo objInfo;
		SHADER_STAGE shaderStages;
		IMAGE_LAYOUT imgLayout = IMAGE_LAYOUT::MaxEnum; /*optional*/
	};

	class DescriptorSheet 
	{
	public:
		DescriptorSheet() = default;
		DescriptorSheet(
			std::initializer_list<DescriptorSheetElementBuilder> descBuilders, 
			const uint16_t maxSetCount)
		{
			using std::get;

			for (auto& descBuilder : descBuilders)
			{
				DescriptorSheetElement element;
				element._descriptorInfo.type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
				element._binding = descBuilder.binding;
				element._descriptorInfo = descBuilder.objInfo;
				if (descBuilder.imgLayout != IMAGE_LAYOUT::MaxEnum) {
					element._overrideImgLayout = descBuilder.imgLayout;
					for (auto& imgInfo : element._descriptorInfo.imageInfos) {
						imgInfo.imageLayout = (VkImageLayout)descBuilder.imgLayout;
					}
				}
				element._shaderStageFlags = (VkShaderStageFlags)descBuilder.shaderStages;
				addSheetElement(element);
			}
			_maxSetCount = maxSetCount;
		}
	public:
		VAL_RETURN_CODE createDescriptorSetLayoutFromSheet(VkDevice device, VkDescriptorSetLayout* layout);

		uint32_t getDescriptorPoolSizes(std::vector<VkDescriptorPoolSize>* descPoolSizes) const;

		VAL_RETURN_CODE allocateSets(VkDevice device, VkDescriptorSet* sets, VkDescriptorSetLayout* setLayouts, uint32_t setCount, VkDescriptorPool pool);

		// allocates a VkDescriptorSet and populates it with the descriptor writes.
		VAL_RETURN_CODE allocateAndWriteSets(VkDevice device, VkDescriptorSet* sets, VkDescriptorSetLayout* setLayouts, uint32_t setCount, VkDescriptorPool pool);

		void updateAndWriteDescriptors(VkDevice device, VkDescriptorSet descriptorSet);

		void updateDescriptor(VkDevice device, const uint32_t descriptorIndex, const uint32_t setIndex);

		bool isInitialized();

	public:
		void addSheetElement(const DescriptorSheetElement& element);

		void insertSheetElement(const uint32_t index, const DescriptorSheetElement& element);

		void setSheetElement(const uint32_t index, const DescriptorSheetElement& element);

		void removeSheetElement(const uint32_t index);

		const tiny_vector<DescriptorSheetElement>& getSheetElements() const;

		inline tiny_vector<VkDescriptorSet>& getVkDescriptorSets()
		{
			return _descriptorSets;
		}
		inline void setVkDescriptorSets(const tiny_vector<VkDescriptorSet>& descriptorSets)
		{
			_descriptorSets = descriptorSets;
		}
		inline const uint16_t& getMaxSetCount() const {
			return _maxSetCount;
		}
		inline void setMaxSetCount(uint16_t setCount) {
			_maxSetCount = setCount;
		}
	private:
		uint16_t _maxSetCount = 1u;
		tiny_vector<VkDescriptorSet> _descriptorSets;
		tiny_vector<VkWriteDescriptorSet> _descriptorWrites;
		tiny_vector<DescriptorSheetElement> _elements;
	};
}

#endif // !NDEBUG