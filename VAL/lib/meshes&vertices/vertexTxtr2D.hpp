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

#include <VAL/lib/meshes&vertices/vertexBase.hpp>
#include <VAL/lib/meshes&vertices/vertexInputAttributeList.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string.h>

namespace val
{
	class VertexTxtr2D : public VertexBase
	{
	public:

		VertexTxtr2D() = default;

		VertexTxtr2D(glm::vec2 _pos, glm::vec2 _texCoord)
		{
			pos = _pos;
			texCoord = _texCoord;
		}
		VertexTxtr2D& operator=(const VertexTxtr2D&) = default;

		bool operator==(const VertexTxtr2D&) const = default;


	public:

		glm::vec2 pos;
		glm::vec2 texCoord;
	public:
		void loadFromAttributes(const glm::vec3& pos, const size_t index, const glm::vec4 color, const glm::vec3& normal, const tiny_vector<glm::vec2>& UVs)
		{
			this->pos.x = pos.x;
			this->pos.y = pos.y;
			if (!UVs.empty())
			{
				this->texCoord = UVs[0];
			}
		}

		static const tiny_vector<VkVertexInputAttributeDescription>& getInputAttributeDescriptions()
		{
			static tiny_vector<VkVertexInputAttributeDescription> r;
			if (r.size() > 0)
				return r;
		
			static const VertexInputAttributeList vertexAttributes = {
				   VertexInputAttribute(0, (VERTEX_ATTRIBUTE_TYPE)getPositionFormat(), offsetof(VertexTxtr2D, pos)),
				   VertexInputAttribute(1, vec2, offsetof(VertexTxtr2D, texCoord)),
			};
			r = vertexAttributes.toVkVertexInputAttributeDescription();
			return r;
		}

		static VkVertexInputBindingDescription getBindingDescription()
		{
			static const VkVertexInputBindingDescription bindingDescription
			{
				.binding = 0,
				.stride = sizeof(VertexTxtr2D),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};
			return bindingDescription;
		}

		inline static VkDeviceSize getStride() {
			return sizeof(VertexTxtr2D);
		}

		inline static VkFormat getPositionFormat() {
			return (VkFormat)VERTEX_ATTRIBUTE_TYPE::vec2;
		}
	};
}


// A hash function is required for vertex deduplication
namespace std {
	template<> struct hash<val::VertexTxtr2D> {
		size_t operator()(val::VertexTxtr2D const& vertex) const {
			return (hash<glm::vec2>()(vertex.pos));
		}
	};
}