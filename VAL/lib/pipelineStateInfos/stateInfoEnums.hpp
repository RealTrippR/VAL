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

#ifndef VAL_CULL_MODE_ENUM_HPP
#define VAL_CULL_MODE_ENUM_HPP

#include <stdint.h>


namespace val {
	/**********************************************************/
	enum class BLEND_POS : uint8_t
	{
		SOURCE,
		DEST
	};
	/**********************************************************/
	enum class CULL_MODE : uint8_t
	{
		NONE = VK_CULL_MODE_NONE,
		FRONT = VK_CULL_MODE_FRONT_BIT,
		BACK = VK_CULL_MODE_BACK_BIT,
		BACK_AND_FRONT = VK_CULL_MODE_FRONT_AND_BACK,
	};
	/**********************************************************/
	enum class TOPOLOGY_MODE : uint8_t {
		LINE = VK_POLYGON_MODE_LINE,
		POINT = VK_POLYGON_MODE_POINT,
		FILL = VK_POLYGON_MODE_FILL
	};
	/**********************************************************/
	enum class PIPELINE_PROPERTY_STATE : uint8_t {
		STATIC,
		DYNAMIC
	};
}

#endif //!VAL_CULL_MODE_ENUM_HPP