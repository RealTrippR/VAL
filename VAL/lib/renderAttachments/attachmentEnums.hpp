/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef VAL_ATTACHMENT_ENUMS_HPP
#define VAL_ATTACHMENT_ENUMS_HPP

namespace val {
	enum RENDER_ATTACHMENT_OPERATION {
		Discard,
		Store,
		Load,
		Clear 
	};

	constexpr VkAttachmentStoreOp RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentStoreOp(const RENDER_ATTACHMENT_OPERATION& opType) {
		switch (opType) {
		case Discard:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		case Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
			break;
		case Load:
			return VK_ATTACHMENT_STORE_OP_STORE;
			break;
		case Clear:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		}
		return VK_ATTACHMENT_STORE_OP_NONE;
	}

	constexpr VkAttachmentLoadOp RENDER_ATTACHMENT_OPERATION_TO_VkAttachmentLoadOp(RENDER_ATTACHMENT_OPERATION& opType) {
		switch (opType) {
		case Discard:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			break;
		case Store:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
			break;
		case Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
			break;
		case Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
			break;
		}
		return VK_ATTACHMENT_LOAD_OP_NONE;
	}
}

#endif // !VAL_ATTACHMENT_ENUMS_HPP