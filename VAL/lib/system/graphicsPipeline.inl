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

#ifndef VAL_GRAPHICS_PIPELINE_CREATE_INFO_INLINE
#define VAL_GRAPHICS_PIPELINE_CREATE_INFO_INLINE

#include <VAL/lib/system/graphicsPipeline.hpp>
#include <VAL/lib/system/renderPass.hpp>

namespace val {
	inline VkPipeline& GraphicsPipeline::getVkPipeline(ValProc& proc) {
		return proc._graphicsPipelines[pipelineIdx];
	}


	inline void GraphicsPipeline::setRenderPassManager(RenderPassManager* passMngr) {
		renderPass = passMngr;
	}

	inline RenderPassManager* GraphicsPipeline::getRenderPassManager() {
		return renderPass;
	}

	inline const RenderPassManager* GraphicsPipeline::getRenderPassManager() const {
		return renderPass;
	}

	inline void GraphicsPipeline::setDynamicRenderingState(DynamicRenderingState* renderState) {
		_dynamicRenderingState = renderState;
	}

	inline DynamicRenderingState* GraphicsPipeline::getDynamicRenderingState() const {
		return _dynamicRenderingState;
	}

	inline VkRenderPass GraphicsPipeline::getVkRenderPass() {
		if (!renderPass) {
			return NULL;
		}
		return renderPass->getVkRenderPass();
	}
}
#endif // !VAL_GRAPHICS_PIPELINE_CREATE_INFO_INLINE
