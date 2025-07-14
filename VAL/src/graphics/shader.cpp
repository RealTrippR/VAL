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

#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	bool Shader::loadFromFile(fs::path filepath) {
		_filepath = fs::absolute(filepath);
#ifndef NDEBUG

		if (!std::filesystem::exists(filepath)) {
			dbg::printWarning("VAL: WARNING: Attempted to load shader from invalid filepath : % ws\n", filepath.c_str());
			return false;
		}

#endif // !NDEBUG

		if (readByteFile(_filepath.string(), &_byteCode)) {
			return true;
		}

		if (_byteCode.size()==0)
		{
			dbg::printWarning("Shader::loadFromFile: bytecodeSize of Shader @ %p is 0.", this);
		}
		return false;
	}

	void Shader::setEntryPoint(const std::string& entryPoint) {
		_entryPoint = entryPoint.c_str();
	}

	void Shader::setEntryPoint(const tiny_vector<char>& entryPoint) {
		_entryPoint = entryPoint;
	}

	const tiny_vector<char>& Shader::getEntryPoint() const {
		return _entryPoint;
	}


	tiny_vector<char>& Shader::getByteCode() noexcept {
		return _byteCode;
	}


	void Shader::deleteByteCode() {
		_byteCode.~tiny_vector();
	}

	const fs::path& Shader::getFilepath() noexcept {
		return _filepath;
	}

	VkShaderStageFlags Shader::getStageFlags() noexcept {
		return _shaderStageFlags;
	}

	void Shader::setStageFlags(const VkShaderStageFlags& stageFlags) {
		_shaderStageFlags  = stageFlags;
	}

	void Shader::setPushConstant(pushConstantHandle* pushConstant) {
		_pushConstant = pushConstant;
		_pushConstant->_stageFlags |= _shaderStageFlags;
	}

	pushConstantHandle* Shader::getPushConstant() noexcept {
		return _pushConstant;
	}

	void Shader::setVertexAttributes(const tiny_vector<VkVertexInputAttributeDescription>& attributes)
	{
		_attributes = attributes;
	}

	void Shader::setVertexAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes)
	{
#ifndef NDBEUG
		if (attributes.size() > _attributes.max_size())
		{
			dbg::printError("Shader::setVertexAttributes: The passed attributes would result in an integer overflow if copied.");
			throw std::runtime_error("Shader::setVertexAttributes: The passed attributes would result in an integer overflow if copied.");
		}
#endif // !NDBEUG

		_attributes.resize((uint32_t)attributes.size());
		for (uint32_t i = 0; i < _attributes.size(); ++i)
		{
			_attributes[i] = attributes[i];
		}
	}

	const tiny_vector<VkVertexInputAttributeDescription>& Shader::getVertexAttributes() noexcept {
		return _attributes;
	}

	void Shader::setBindingDescription(const VkVertexInputBindingDescription& bindingDescription)
	{
		_bindings.resize(1);
		_bindings[0] = bindingDescription;
	}

	void Shader::setBindingDescriptions(const tiny_vector<VkVertexInputBindingDescription>& bindingDescriptions)
	{
		_bindings = bindingDescriptions;
	}

	const tiny_vector<VkVertexInputBindingDescription>& Shader::getBindingDescriptions() noexcept
	{
		return _bindings;
	}
}