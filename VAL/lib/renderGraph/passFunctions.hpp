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

#ifndef VAL_PASS_FUNCTIONS_HPP
#define VAL_PASS_FUNCTIONS_HPP

#include <VAL/lib/system/VAL_PROC.hpp>

static val::VAL_PROC* pass_proc;

namespace val {
	inline void setPipeline(graphicsPipelineCreateInfo& pipeline);

	inline void setViewport(const VkViewport& viewport);

	inline void setScissor(const VkRect2D& scissor);

	inline void setScissor(const VkExtent2D& scissor);

	//inline void setLineWidth();

	//inline void setBlendConstants();

	//inline void setTopologyMode();

	//inline void setCullMode();

	//inline void setDepthBias();

	//inline void copyImage(val::image& dst, val::image& src);

	//inline void copyImage(val::image& dst, val::image& src, val::imageCopyInfo& copyInfo);

	//inline void copyBuffer(val::image& dst, val::image& src);

	inline void setVertexBuffer(val::buffer& buffer);

	//inline void setVertexBuffers();

	inline void setIndexBuffer(val::buffer& buffer);

	//inline void setIndexAndVertexBuffers();
}

#include <VAL/lib/renderGraph/passFunctionDefinitions.hpp>


#endif // !VAL_PASS_FUNCTIONS_HPP