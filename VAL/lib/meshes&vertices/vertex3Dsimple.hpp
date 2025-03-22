#ifndef VAL_VERTEX_3D_COLORED_HPP
#define VAL_VERTEX_3D_COLORED_HPP

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// to allow hasing for the vertex3D struct
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace val {
    struct vertex3Dsimple
    {
        glm::vec3 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(vertex3Dsimple);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
            attributeDescriptions.resize(2);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(vertex3Dsimple, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(vertex3Dsimple, color);

            return attributeDescriptions;
        }

        bool operator==(const vertex3Dsimple& other) const {
            return pos == other.pos && color == other.color;
        }
    };
}

namespace std {
    using namespace val;
    template<> struct hash<vertex3Dsimple> {
        size_t operator()(vertex3Dsimple const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
        }
    };

}

#endif // !VAL_VERTEX_3D_COLORED_HPP