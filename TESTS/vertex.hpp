#ifndef TEST_VERTEX_HPP
#define TEST_VERTEX_HPP


#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VAL/lib/meshes&vertices/VertexInputAttribute.hpp>
#include <VAL/lib/meshes&vertices/vertexInputAttributeList.hpp>

namespace res {

    struct vertex 
    {
        glm::vec2 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec3 normal = { 0,0,0 };

        static VkVertexInputBindingDescription getBindingDescription() 
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() 
        {
            using namespace val;
            VertexInputAttributeList vertexAttributes = {
                VertexInputAttribute(0, vec2, offsetof(vertex, pos)),
                VertexInputAttribute(1, vec3, offsetof(vertex, color)),
                VertexInputAttribute(2, vec2, offsetof(vertex, texCoord)),
                VertexInputAttribute(3, vec3, offsetof(vertex, normal))
            };
         
            return vertexAttributes.toVkVertexInputAttributeDescriptionStdVector();
                


        /*  attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(vertex, pos);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(vertex, color);
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(vertex, texCoord);*/
        }
    };
}

#endif // !TEST_VERTEX_HPP
