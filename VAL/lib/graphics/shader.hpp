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

#ifndef VAL_SHADER_HPP
#define VAL_SHADER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>
#include <VAL/lib/system/specializationConstant.hpp>
#include <VAL/lib/system/sampler.hpp>

#include <VAL/lib/system/pushDescriptor.hpp>

#include <optional>
#include <string.h>
#include <stdexcept>
#include <filesystem>

/*THE DIFFERENT TYPES OF SHADERS SUPPORTED BY VULKAN
VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
VK_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
VK_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
VK_SHADER_STAGE_ALL = 0x7FFFFFFF,

// And raytracing....

*/

namespace val {
	class VAL_PROC; // forward declaration
	namespace fs = std::filesystem;

	class Shader {
	public:
		Shader() = default;

		Shader(fs::path filepath, VkShaderStageFlags shaderStageFlags, std::string entryPoint = "main") {
			loadFromFile(filepath);
			_entryPoint = entryPoint.c_str();
			_shaderStageFlags = shaderStageFlags;
		}

		Shader(fs::path filepath, SHADER_STAGE shaderStageFlags, std::string entryPoint = "main")
			: Shader(filepath, static_cast<VkShaderStageFlags>(shaderStageFlags), entryPoint) {}
		
		~Shader() {
			discardSpecalizationConstants();
		}

	public:
		// returns true if the shader was succesfully loaded
		bool loadFromFile(fs::path filepath);

		void setEntryPoint(const std::string& entryPoint);

		void setEntryPoint(const tiny_vector<char>& entryPoint);

		const tiny_vector<char>& getEntryPoint() const;

		const fs::path& getFilepath() noexcept;

		tiny_vector<char>& getByteCode() noexcept;

		void deleteByteCode();


		void setStageFlags(const VkShaderStageFlags& stageFlags);

		VkShaderStageFlags getStageFlags() noexcept;


		void setPushConstant(pushConstantHandle* pushConstant);

		pushConstantHandle* getPushConstant() noexcept;

		void setVertexAttributes(const tiny_vector<VkVertexInputAttributeDescription>& attributes);

		void setVertexAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes);

		const tiny_vector<VkVertexInputAttributeDescription>& getVertexAttributes() noexcept;


		void setBindingDescription(const VkVertexInputBindingDescription& bindingDescription);

		void setBindingDescriptions(const tiny_vector<VkVertexInputBindingDescription>& bindingDescriptions);

		const tiny_vector<VkVertexInputBindingDescription>& getBindingDescriptions() noexcept;


		void addSpecializationConstant(uint16_t id, const void* value, uint16_t valueSize, uint16_t offset = 0u);

		uint32_t getSpecializationEntriesDataSize() const;

		void discardSpecalizationConstants();
	public:
		tiny_vector<char> _entryPoint = "main";
		tiny_vector<char> _byteCode;
		fs::path _filepath;

		//tiny_vector<VkDescriptorSetLayoutBinding> _pushDescriptorLayoutBindings;

		pushConstantHandle* _pushConstant = NULL;
		VkShaderStageFlags _shaderStageFlags;

		tiny_vector<VkVertexInputAttributeDescription> _attributes;
		tiny_vector<VkVertexInputBindingDescription> _bindings;


		tiny_vector<VkSpecializationMapEntry> specializationEntries;
		void* specializationData = NULL;
	};
}

#endif // !VAL_SHADER_HPP