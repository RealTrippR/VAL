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

#include <VAL/lib/system/system_utils.hpp>

#ifndef VAL_VERTEX_BASE_HPP
#define VAL_VERTEX_BASE_HPP

namespace val
{
	class VertexBase
	{
	public:
		VertexBase& operator=(const VertexBase&) = default;

		bool operator==(const VertexBase&) const = default;

		virtual ~VertexBase() = default;
	public:
		virtual void loadFromAttributes(const glm::vec3& pos, const size_t index, const glm::vec4 color, const glm::vec3& normal, const tiny_vector<glm::vec2>& UVs)
		{}

		inline static VkDeviceSize getStride() {
			dbg::printWarning("VertexBase::getPositionFormat was called. This function is of a base class and should never be called. This likely indicates an error in the program.");
			return 0u;
		}

		inline static VkFormat getPositionFormat() {
			dbg::printWarning("VertexBase::getPositionFormat was called. This function is of a base class and should never be called. This likely indicates an error in the program.");
			return VK_FORMAT_UNDEFINED;
		}

		static const tiny_vector<VkVertexInputAttributeDescription>& getInputAttributeDescriptions() {
			dbg::printWarning("VertexBase::getInputAttributeDescriptions was called. This function is of a base class and should never be called. This likely indicates an error in the program.");
			return {}; 
		};

		static VkVertexInputBindingDescription getBindingDescription() {
			dbg::printWarning("VertexBase::VkVertexInputBindingDescription was called. This function is of a base class and should never be called. This likely indicates an error in the program.");
			return {};
		};

	};
}

#endif // !VAL_VERTEX_BASE