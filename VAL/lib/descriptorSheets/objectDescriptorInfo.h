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

#ifndef VAL_OBJECT_DESCRIPTOR_INFO_H
#define VAL_OBJECT_DESCRIPTOR_INFO_H

#include <vulkan/vulkan_core.h>
#include <optional>
#include <VAL/lib/ext/tiny_vector.hpp>

namespace val
{
	struct ObjectDescriptorInfo
	{
		void* valObject = NULL; // object which the objectDescriptorInfo was created from
		tiny_vector<VkDescriptorImageInfo> imageInfos;
		tiny_vector<VkDescriptorBufferInfo> bufferInfos;
		tiny_vector<VkBufferView> texelBufferViews;
		VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
		uint32_t arrCount = 1u;
		void(*updateDataCallback)(ObjectDescriptorInfo*);
		void* pNext = NULL; // used by VK_ACCELERATION_STRUCTURE


		inline void updateWithDataCallback()
		{
			if (!updateDataCallback)
			{
				dbg::printError("ObjectDescriptorInfo::updateWithDataCallback: updateDataCallback of ObjectDescriptorInfo @ %p is NULL.", this);
			}
			else {
				updateDataCallback(this);
			}
		}
	};
}

#endif // !VAL_OBJECT_DESCRIPTOR_INFO_H