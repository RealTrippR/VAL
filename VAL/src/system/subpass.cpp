#include <VAL/lib/system/subpass.hpp>
#include <VAL/lib/system/renderPass.hpp>

namespace val
{
	void subpass::update() {
		// update color attachments
		_colorAttachments.clear();
		_resolveAttachments.clear();
		_inputAttachments.clear();

#ifndef NDEBUG
		// this value should NEVER exceed 1, as each subpass can only have 1 depth stencil
		uint32_t _depthAttacmentDebugCount = 0;
#endif // !NDEBUG

		for (uint32_t i = 0; i < _attachments.size(); ++i) {
			renderAttachment& attachment = *(_attachments[i]);
			{
				// handle color attachment
				renderAttachment* asColorAttachment = dynamic_cast<colorAttachment*>(&attachment);
				if (asColorAttachment != NULL) {
					_colorAttachments.resize(_colorAttachments.size() + 1);
					_colorAttachments.back().attachment = i;
					_colorAttachments.back().layout = attachment.getRefLayout();
					continue;
				}
			}

			{
				// handle depth attachment
				renderAttachment* asDepthAttachment = dynamic_cast<depthAttachment*>(&attachment);
				if (asDepthAttachment != NULL) {
				#ifndef NDEBUG
					_depthAttacmentDebugCount++;
				#endif // !NDEBUG
					VkAttachmentReference tmp{};
					tmp.attachment = i;
					tmp.layout = asDepthAttachment->getRefLayout();
					_depthStencilAttachment = tmp;
					continue;
				}
			}
			{
				// handle resolve attachment
				renderAttachment* asResolveAttachment = dynamic_cast<resolveAttachment*>(&attachment);
				if (asResolveAttachment != NULL) {
					_resolveAttachments.resize(_resolveAttachments.size() + 1);
					_resolveAttachments.back().attachment = i;
					_resolveAttachments.back().layout = attachment.getRefLayout();
					continue;
				}
			}
			{
				// handle input attachment
				renderAttachment* asInputAttachment = dynamic_cast<inputAttachment*>(&attachment);
				if (asInputAttachment != NULL) {
					_inputAttachments.resize(_inputAttachments.size() + 1);
					_inputAttachments.back().attachment = i;
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



	void subpass::bindAttachment(renderAttachment* attachment) {
		_attachments.push_back(attachment);
		//_rpMngr->_attachments.push_back(attachment);
	}

	void subpass::removeAttachment(renderAttachment* attachment) {
		_attachments.erase(std::remove(_attachments.begin(), _attachments.end(), attachment), _attachments.end());
	}

	void subpass::attachToRenderPassManager() {
		_rpMngr->_subpasses.push_back(this);
	}

}