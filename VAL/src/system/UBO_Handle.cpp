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

#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {

	void* UBO_Handle::getData(VAL_PROC& proc) {
		return (char*)_arrSubset->getMappedDataOfFrame(proc._currentFrame) + _offset;
	}

	void* UBO_Handle::getData(VAL_PROC& proc, const uint8_t frameIdx) {
		return (char*)_arrSubset->getMappedDataOfFrame(frameIdx) + _offset;
	}

	VkBuffer UBO_Handle::getBuffer(VAL_PROC& proc) {
		return _arrSubset->_vkBuff;
	}

	void UBO_Handle::update(VAL_PROC& proc, void* data) {
		uint8_t* tmp = (uint8_t*)_arrSubset->getMappedDataOfFrame(proc._currentFrame);
		memcpy((uint8_t*)_arrSubset->getMappedDataOfFrame(proc._currentFrame) + _offset, data, _size);
	}

	void UBO_Handle::update(VAL_PROC& proc, void* data, const uint8_t frameIdx) {
		memcpy((char*)_arrSubset->getMappedDataOfFrame(frameIdx) + _offset, data, _size);
	}

	uboArraySubset* UBO_Handle::getUBOarraySubset(VAL_PROC& proc) {
		return _arrSubset;
	}

	VkMemoryPropertyFlags UBO_Handle::getMemoryPropertyFlags() {
		if (GPU_ONLY) {
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		else if (CPU_GPU)
		{
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}
	}
}