#ifndef VAL_FENCE_HPP
#define VAL_FENCE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class Fence
	{
	public:
		inline VAL_RETURN_CODE create(VkDevice device);

		inline void destroy(VkDevice device);

		inline void wait(VkDevice device);

		inline void reset(VkDevice device);

		operator VkFence() {
			return _fence;
		}

		operator const VkFence&() const {
			return _fence;
		}
	private:
		VkFence _fence;
	};
}

#endif