#ifndef VAL_FENCE_INL
#define VAL_FENCE_INL
#include <VAL/lib/system/Fence.hpp>

namespace val
{
	inline VAL_RETURN_CODE Fence::create(VkDevice device)
	{
		VkFenceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = 0x0,
			.flags = 0x0
		};

		
		if (vkCreateFence(device, &createInfo, VK_NULL_HANDLE, &_fence) != VK_SUCCESS) {
			return VAL_FAILURE;
		}
		return VAL_SUCCESS;
	}

	inline void Fence::destroy(VkDevice device)
	{
		vkDestroyFence(device, _fence, VK_NULL_HANDLE);
		_fence = NULL;
	}

	inline void Fence::wait(VkDevice device) {
		vkWaitForFences(device, 1, &_fence, true, UINT64_MAX);
	}

	inline void Fence::reset(VkDevice device) {
		vkResetFences(device, 1, &_fence);
	}
}

#endif //!VAL_FENCE_INL