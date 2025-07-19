#ifndef VAL_SUBPASS_HPP
#define VAL_SUBPASS_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/renderAttachments/renderAttachment.hpp>
#include <VAL/lib/renderAttachments/colorAttachment.hpp>
#include <VAL/lib/renderAttachments/depthAttachment.hpp>
#include <VAL/lib/renderAttachments/inputAttachment.hpp>
#include <VAL/lib/renderAttachments/resolveAttachment.hpp>
#include <tiny/optional.h>

namespace val
{
	constexpr VkAttachmentReference NullAttachment = { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED };

	class RenderPassManager; // forward declaration

	class Subpass {
	public:
		Subpass(RenderPassManager& rpManager) : _rpMngr(&rpManager) {
			attachToRenderPassManager();
		};
		Subpass(RenderPassManager& rpManager, PIPELINE_TYPE pipelineBindPoint) : _rpMngr(&rpManager) {
			_subpassDesc.pipelineBindPoint = PIPELINE_TYPE_To_VkPipelineBindPoint(pipelineBindPoint);
			attachToRenderPassManager();
		}

	public:
		void update();

		void setSrcDependency(const PIPELINE_STAGE stageMask, const ACCESS_FLAGS accessMask);

		PIPELINE_STAGE getSrcDependencyPipelineStageMask();

		ACCESS_FLAGS getSrcDependencyAccessMask();

		void setDstDependency(const PIPELINE_STAGE destMask, const ACCESS_FLAGS accessMask);

		PIPELINE_STAGE getDstDependencyPipelineStageMask();

		ACCESS_FLAGS getDstDependencyAccessMask();

	public:
		void bindAttachment(renderAttachment* attachment);

		//void removeAttachment(renderAttachment* attachment);

	protected:
		friend ValProc;
		friend renderAttachment;
		friend RenderPassManager;
		/******************************************/
		RenderPassManager* _rpMngr;
		/******************************************/
		VkSubpassDescription _subpassDesc{};
		ACCESS_FLAGS _srcDependencyAccessMask;
		PIPELINE_STAGE _srcDependencyStageMask;
		ACCESS_FLAGS _dstDependencyAccessMask;
		PIPELINE_STAGE _dstDependencyStageMask;

		tiny_vector<VkAttachmentReference> _attachmentReferences;
		/******************************************/
		tiny_vector<VkAttachmentReference> _colorAttachments;
		std::optional<VkAttachmentReference> _depthStencilAttachment; // subpasses can only have 1 depth attachment
		tiny_vector<VkAttachmentReference> _resolveAttachments;
		tiny_vector<VkAttachmentReference> _inputAttachments;
	protected:
		void attachToRenderPassManager();
	};
}

#endif // !VAL_SUBPASS_HPP