#ifndef VAL_QUEUE_FLAGS_HPP
#define VAL_QUEUE_FLAGS_HPP

#include <stdint.h>
#include <VAL/lib/classEnumBitOps.hpp>
#include <ExternalLibraries/Vulkan/Include/vulkan/vulkan_core.h>

namespace val
{
	enum class QUEUE_FLAGS : uint16_t
	{
		Graphics = VK_QUEUE_GRAPHICS_BIT,
		Compute = VK_QUEUE_COMPUTE_BIT,
		Transfer = VK_QUEUE_TRANSFER_BIT,
		SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,
		Protected = VK_QUEUE_PROTECTED_BIT,
		VideoDecode = VK_QUEUE_VIDEO_DECODE_BIT_KHR,
		VideoEncode = VK_QUEUE_VIDEO_ENCODE_BIT_KHR,
		OpticalFlowNVIDIA = VK_QUEUE_OPTICAL_FLOW_BIT_NV,
	};

#ifndef VAL_QUEUE_FLAGS_BITWISE_OPS
#define VAL_QUEUE_FLAGS_BITWISE_OPS
	DEF_ENUM_BITWISE_OPERATORS(QUEUE_FLAGS);
#endif
}

#endif // !VAL_QUEUE_FLAGS_HPP