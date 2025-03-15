#ifndef VAL_RENDER_ATTACHMENT_HPP
#define VAL_RENDER_ATTACHMENT_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/attachmentEnums.hpp>

namespace val {
	class renderAttachment
	{
	public:
		virtual VkAttachmentDescription toVkAttachmentDescription() {
			assert(0);
		}

		uint8_t getMSAA_Samples() {
			return uint8_t(_MSAA_Samples);
		}

		void setMSAA_Samples(uint8_t sampleCount /*Must be a power of 2*/) {
#ifndef NDEBUG
			if ((sampleCount & (sampleCount - 1)) != 0) {
				printf("VAL: ERROR: Invalid sample count of %u; the sample count must be a power of two!\n", sampleCount);
				throw std::runtime_error("VAL: ERROR: Invalid sample count; the sample count must be a power of two!");

			}
#endif // !NDEBUG

			_MSAA_Samples = VkSampleCountFlags(sampleCount);
		}

		void setLoadOperation(RENDER_ATTACHMENT_OPERATION_TYPE opType) {

		}

		static constexpr VkImageLayout getRefLayout() {
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	protected:
		VkSampleCountFlags _MSAA_Samples = 0u;
		RENDER_ATTACHMENT_OPERATION_TYPE _loadOp = CLEAR;
		RENDER_ATTACHMENT_OPERATION_TYPE _storeOp = STORE;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilLoadOp = DISCARD;
		RENDER_ATTACHMENT_OPERATION_TYPE _stencilStoreOp = DISCARD;
		VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	};
}
#endif // !VAL_RENDER_ATTACHMENT_HPP