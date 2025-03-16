#ifndef VAL_ATTACHMENT_ENUMS_HPP
#define VAL_ATTACHMENT_ENUMS_HPP

namespace val {
	enum RENDER_ATTACHMENT_OPERATION_TYPE {
		DISCARD,
		STORE,
		LOAD,
		CLEAR 
	};

	constexpr VkAttachmentStoreOp RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentStoreOp(const RENDER_ATTACHMENT_OPERATION_TYPE& opType) {
		switch (opType) {
		case DISCARD:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		case STORE:
			return VK_ATTACHMENT_STORE_OP_STORE;
			break;
		case LOAD:
			return VK_ATTACHMENT_STORE_OP_STORE;
			break;
		case CLEAR:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		}
	}

	constexpr VkAttachmentLoadOp RENDER_ATTACHMENT_OPERATION_TYPE_TO_VkAttachmentLoadOp(RENDER_ATTACHMENT_OPERATION_TYPE& opType) {
		switch (opType) {
		case DISCARD:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			break;
		case STORE:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
			break;
		case LOAD:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
			break;
		case CLEAR:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
			break;
		}
	}
}

#endif // !VAL_ATTACHMENT_ENUMS_HPP