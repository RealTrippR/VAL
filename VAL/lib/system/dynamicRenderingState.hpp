#ifndef VAL_DYNAMIC_RENDERING_STATE
#define VAL_DYNAMIC_RENDERING_STATE

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/colorAttachment.hpp>
#include <VAL/lib/renderAttachments/depthAttachment.hpp>
#include <VAL/lib/renderAttachments/inputAttachment.hpp>

namespace val
{
	class DynamicRenderingState
	{
	public:
		inline void setColorAttachments(const tiny_vector<ColorAttachment*>& attachments) {
			_colorAttachments = attachments;
			_colorAttachmentFormats.resize(_colorAttachments.size());
			for (uint32_t i = 0; i < _colorAttachmentFormats.size(); ++i) {
				_colorAttachmentFormats[i] = _colorAttachments[i]->getImgFormat();
			}
		}

		inline const tiny_vector<ColorAttachment*>& getColorAttachments() const {
			return _colorAttachments;
		}

		inline void setDepthAttachment(DepthAttachment* attachment) {
			_depthAttachment = attachment;
		}

		inline DepthAttachment* getDepthAttachment() const {
			return _depthAttachment;
		}

		inline VkPipelineRenderingCreateInfo* asVkPipelineRenderingCreateInfo() {
			_vkPipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
			_vkPipelineRenderingCreateInfo.pNext = VK_NULL_HANDLE;
			_vkPipelineRenderingCreateInfo.colorAttachmentCount = _colorAttachmentFormats.size();
			_vkPipelineRenderingCreateInfo.pColorAttachmentFormats = _colorAttachmentFormats.data();
			if (_depthAttachment != NULL) {
				_vkPipelineRenderingCreateInfo.depthAttachmentFormat = _depthAttachment->getImgFormat();
			}
			_vkPipelineRenderingCreateInfo.viewMask = 0x0; // only set when multiview rendering is enabled
			return &_vkPipelineRenderingCreateInfo;
		}

		inline const VkRenderingInfo& asVkRenderingInfo() {
			return _renderingInfo;
		}

	public:
		operator const VkRenderingInfo*() const {
			return &_renderingInfo;
		}

		operator const VkRenderingInfo& () const  {
			return _renderingInfo;
		}

	private:
		VkPipelineRenderingCreateInfo _vkPipelineRenderingCreateInfo;
		VkRenderingInfo _renderingInfo;
		tiny_vector<VkFormat> _colorAttachmentFormats;
		tiny_vector<ColorAttachment*> _colorAttachments;
		DepthAttachment* _depthAttachment = NULL;
	};
}

#endif // !VAL_DYNAMIC_RENDERING_STATE