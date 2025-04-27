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