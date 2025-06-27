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

#include <vector>

#ifndef VAL_FBX_MATERIAL_PROPERTY_HPP
#define VAL_FBX_MATERIAL_PROPERTY_HPP

namespace val {
	enum class FBX_MATERIAL_PROPERTY 
	{
		diffuse,
		normal,
		specular,
		metallic,
		roughness,
		bump,
		opacity,
		emissive,
		ambient
	};

	inline std::vector<const char*> FBX_MATERIAL_PROPERTY_toStringList(enum FBX_MATERIAL_PROPERTY prop)
	{
		switch (prop)
		{
		case FBX_MATERIAL_PROPERTY::diffuse:
			return { "diffuse_color", "albedo", "DiffuseColor", "base_color_texture"};
		case FBX_MATERIAL_PROPERTY::normal:
			return { "normal_map", "NormalMap", "normalmap_texture"};
		case FBX_MATERIAL_PROPERTY::specular:
			return { "specular_color", "SpecularColor"};
		case FBX_MATERIAL_PROPERTY::metallic:
			return { "metallic", "metalness", "metallic_texture"};
		case FBX_MATERIAL_PROPERTY::roughness:
			return { "roughness" };
		case FBX_MATERIAL_PROPERTY::bump:
			return { "bump" };
		case FBX_MATERIAL_PROPERTY::opacity:
			return { "opacity" };
		case FBX_MATERIAL_PROPERTY::emissive:
			return { "emissive", "emissive_color" };
		case FBX_MATERIAL_PROPERTY::ambient:
			return { "ambient", "ambient_color" };
		default: return {};
		}
	}
}
#endif // !VAL_FBX_MATERIAL_PROPERTY_HPP