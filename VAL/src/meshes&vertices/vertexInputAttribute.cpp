#include <VAL/lib/meshes&vertices/vertexInputAttribute.hpp>

namespace val {
	tiny_vector<VkVertexInputAttributeDescription> VertexInputAttribute::toVkVertexInputAttributeDescriptions() const
	{
		tiny_vector<VkFormat> formats = VERTEX_ATTRIBUTE_TYPE_toVkFormat(type);

		tiny_vector< VkVertexInputAttributeDescription> vertexInputAttributeDescs(formats.size());

		for (uint32_t i = 0; i < formats.size(); ++i)
		{
			vertexInputAttributeDescs[i].format = formats[i];
			vertexInputAttributeDescs[i].location = location + i;
			vertexInputAttributeDescs[i].binding = binding;
			vertexInputAttributeDescs[i].offset = offset;
		}

		return vertexInputAttributeDescs;
	}
}