#include <VAL/lib/system/subpass.hpp>
#include <VAL/lib/system/renderPass.hpp>

namespace val
{

	/*
	void subpass::update(uint32_t startingAttachmentIndex) {
		// update color attachments
		_colorAttachments.clear();
		_resolveAttachments.clear();
		_inputAttachments.clear();

#ifndef NDEBUG
		// this value should NEVER exceed 1, as each subpass can only have 1 depth stencil
		uint32_t _depthAttacmentDebugCount = 0;
#endif // !NDEBUG

		for (uint32_t i = 0; i < _attachments.size(); ++i) {
			renderAttachment& attachment = *(_rpMngr->_attachments[(_attachments[i]).attachment]);
			{
				// handle color attachment
				colorAttachment* asColorAttachment = dynamic_cast<colorAttachment*>(&attachment);
				if (asColorAttachment != NULL) {
					_colorAttachments.resize(_colorAttachments.size() + 1);
					if (asColorAttachment->unused()) {
						_colorAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
					} else {
						//_colorAttachments.back().attachment = i + startingAttachmentIndex;
					}
					_colorAttachments.back().layout = attachment.getRefLayout();
					continue;
				}
			}
			{
				// handle depth attachment
				depthAttachment* asDepthAttachment = dynamic_cast<depthAttachment*>(&attachment);
				if (asDepthAttachment != NULL) {
				#ifndef NDEBUG
					_depthAttacmentDebugCount++;
				#endif // !NDEBUG
					VkAttachmentReference tmp{};
					if (asDepthAttachment->unused()) {
						tmp.attachment = VK_ATTACHMENT_UNUSED;
					} else {
						tmp.attachment = i + startingAttachmentIndex;
					}
					tmp.layout = asDepthAttachment->getRefLayout();
					_depthStencilAttachment = tmp;
					continue;
				}
			}
			{
				// handle resolve attachment
				resolveAttachment* asResolveAttachment = dynamic_cast<resolveAttachment*>(&attachment);
				if (asResolveAttachment != NULL) {
					_resolveAttachments.resize(_resolveAttachments.size() + 1);
					if (asResolveAttachment->unused()) {
						_inputAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
					}
					else {
						_resolveAttachments.back().attachment = i + startingAttachmentIndex;
					}
					_resolveAttachments.back().layout = attachment.getRefLayout();
					continue;
				}
			}
			{
				// handle input attachment
				inputAttachment* asInputAttachment = dynamic_cast<inputAttachment*>(&attachment);
				if (asInputAttachment != NULL) {
					_inputAttachments.resize(_inputAttachments.size() + 1);
					if (asInputAttachment->unused()) {
						_inputAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
					}
					else {
						_inputAttachments.back().attachment = i + startingAttachmentIndex;
					}
					_inputAttachments.back().layout = attachment.getRefLayout();
					continue;
				}
			}
		}

#ifndef NDEBUG
		if (_depthAttacmentDebugCount > 1)
		{
			printf("VAL: WARNING: Attempted to updated subpass at address %h with more than 1 depth stencil. This is invalid as only 1 depth stencil can be bound to a subpass.\n");
		}
#endif // !NDEBUG

		// Color attachments
		_subpassDesc.pColorAttachments = _colorAttachments.data();
		_subpassDesc.colorAttachmentCount = _colorAttachments.size();

		// Depth attachment
		if (_depthStencilAttachment.has_value()) {
			_subpassDesc.pDepthStencilAttachment = &(_depthStencilAttachment.value());
		}

		// Resolve attachments (1 for every multisampled color attachment; there won't always be a resolve attachment for every color attachment)
		_subpassDesc.pResolveAttachments = _resolveAttachments.data();

		// Input attachments
		_subpassDesc.pInputAttachments = _inputAttachments.data();
		_subpassDesc.inputAttachmentCount = _inputAttachments.size();
	}
	*/

	void Subpass::update() {

		// Color attachments
		_subpassDesc.pColorAttachments = _colorAttachments.data();
		_subpassDesc.colorAttachmentCount = _colorAttachments.size();
		
		// Depth attachment
		if (_depthStencilAttachment.has_value()) {
			_subpassDesc.pDepthStencilAttachment = &(_depthStencilAttachment.value());
		}

		// Resolve attachments (1 for every multisampled color attachment; there won't always be a resolve attachment for every color attachment)
		_subpassDesc.pResolveAttachments = _resolveAttachments.data();

		// Input attachments
		_subpassDesc.pInputAttachments = _inputAttachments.data();
		_subpassDesc.inputAttachmentCount = (uint32_t)_inputAttachments.size();
	}

	void Subpass::setSrcDependency(const PIPELINE_STAGE stageMask, const ACCESS_FLAGS accessMask) {
		_srcDependencyStageMask = stageMask;
		_srcDependencyAccessMask = accessMask;
	}

	PIPELINE_STAGE Subpass::getSrcDependencyPipelineStageMask() {
		return _srcDependencyStageMask;
	}

	ACCESS_FLAGS Subpass::getSrcDependencyAccessMask() {
		return _srcDependencyAccessMask;
	}

	void Subpass::setDstDependency(const PIPELINE_STAGE destMask, const ACCESS_FLAGS accessMask) {
		_dstDependencyStageMask = destMask;
		_dstDependencyAccessMask = accessMask;
	}

	PIPELINE_STAGE Subpass::getDstDependencyPipelineStageMask() {
		return _dstDependencyStageMask;
	}

	ACCESS_FLAGS Subpass::getDstDependencyAccessMask() {
		return _dstDependencyAccessMask;
	}

	void Subpass::bindAttachment(renderAttachment* attachment) {
		// will not be added if it's already in the list
		uint32_t idx = _rpMngr->addAttachment(attachment);
		
		// create attachment reference
		VkAttachmentReference& vkRef = _attachmentReferences.emplace_back();
		vkRef.attachment = idx;
		vkRef.layout = attachment->getRefLayout();

		/***********************************************/
		{
			// handle color attachment
			ColorAttachment* asColorAttachment = dynamic_cast<ColorAttachment*>(attachment);
			if (asColorAttachment) {
				_colorAttachments.push_back(vkRef);
				return;
			}
		}
		{
			// handle depth attachment
			DepthAttachment* asDepthAttachment = dynamic_cast<DepthAttachment*>(attachment);
			if (asDepthAttachment) {
#ifndef NDEBUG
				if (_depthStencilAttachment.has_value()) {
					dbg::printWarning("Subpass::bindAttachment: The depth stencil attachment of subpass at address %p was overwritten during bindAttachment(), this is likely not intended behavior.\n", this);
				}
#endif // !NDEBUG
				_depthStencilAttachment = vkRef;

				return;
			}
		}
		{
			// handle resolve attachment
			ResolveAttachment* asResolveAttachment = dynamic_cast<ResolveAttachment*>(attachment);
			if (asResolveAttachment) {
				_resolveAttachments.push_back(vkRef);
				return;
			}
		}
		{
			// handle input attachment
			InputAttachment* asInputAttachment = dynamic_cast<InputAttachment*>(attachment);
			if (asInputAttachment) {
				_inputAttachments.push_back(vkRef);
				return;
			}
		}
	}

	/*
	void subpass::removeAttachment(renderAttachment* attachment) {
		_attachments.erase(std::remove(_attachments.begin(), _attachments.end(), attachment), _attachments.end());
	}
	*/

	void Subpass::attachToRenderPassManager() {
		_rpMngr->_subpasses.push_back(this);
	}

}