#ifndef VAL_SUBPASS_HPP
#define VAL_SUBPASS_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/renderAttachment.hpp>
#include <VAL/lib/renderAttachments/colorAttachment.hpp>
#include <VAL/lib/renderAttachments/depthAttachment.hpp>
#include <VAL/lib/renderAttachments/inputAttachment.hpp>
#include <VAL/lib/renderAttachments/resolveAttachment.hpp>

namespace val
{
	class renderPassManager; // forward declaration

	class subpass {
	public:
		subpass(renderPassManager& rpManager) : _rpMngr(&rpManager) {
			//_rpMngr->_subpasses.push_back(this);
			attachToRenderPassManager();
		};
		subpass(renderPassManager& rpManager, PIPELINE_TYPE pipelineBindPoint) : _rpMngr(&rpManager) {
			_subpassDesc.pipelineBindPoint = PIPELINE_TYPE_To_VkPipelineBindPoint(pipelineBindPoint);
			//_rpMngr->_subpasses.push_back(this);
			attachToRenderPassManager();
		}
	public:

		void update(uint32_t startingAttachmentIndex);

	public:
		void bindAttachment(renderAttachment* attachment);
		void removeAttachment(renderAttachment* attachment);

	public:
		renderPassManager* _rpMngr;
		/******************************************/
		VkSubpassDescription _subpassDesc{};
		VkSubpassDependency _dependency{};
		std::vector<renderAttachment*> _attachments;
		/******************************************/
		std::vector<VkAttachmentReference> _colorAttachments;
		std::optional<VkAttachmentReference> _depthStencilAttachment; // subpasses can only have 1 depth attachment
		std::vector<VkAttachmentReference> _resolveAttachments;
		std::vector<VkAttachmentReference> _inputAttachments;
	protected:
		void attachToRenderPassManager();
	};
}

#endif // !VAL_SUBPASS_HPP