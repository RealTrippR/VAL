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

#ifndef VAL_DEBUG_UTILS_HPP
#define VAL_DEBUG_UTILS_HPP


#include <VAL/lib/VALreturnCode.h>
#include <vulkan/vulkan_core.h>
#include <string>

namespace val
{
	namespace dbg 
	{
		VAL_RETURN_CODE initDbgUtilsForDevice(VkDevice device);

		VAL_RETURN_CODE deinitDbgUtilsForDevice(VkDevice device);

		void recordVkObjectCreation(VkDevice device, void* obj);

		void recordVkObjectDestruction(VkDevice device, void* obj);

		void setBreakOnObjectCreation(void* obj);

		void setBreakOnObjectDestruction(void* obj);

		void removeBreakOnObjectCreation(void* obj);

		void removeBreakOnObjectDestruction(void* obj);

		VAL_RETURN_CODE beginPassMarkerRegion(VkDevice device, VkCommandBuffer cmdbuff, std::string markerName, const float color[4]);

		VAL_RETURN_CODE endPassMarkerRegion(VkDevice device, VkCommandBuffer cmdbuff);

		VAL_RETURN_CODE insertPassMarker(VkDevice device, VkCommandBuffer cmdbuff, std::string markerName, const float color[4]);

		//VAL_RETURN_CODE nameVkObject(VkDevice device, std::string name);
		//VAL_RETURN_CODE tagVkObject(VkDevice, std::string tag);
	}
}

#endif // !VAL_DEBUG_UTILS_HPP