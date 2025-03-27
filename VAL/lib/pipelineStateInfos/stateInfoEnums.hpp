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
	enum class CULL_MODE_ENUMS : uint8_t
	{
		NONE = VK_CULL_MODE_NONE,
		FRONT = VK_CULL_MODE_FRONT_BIT,
		BACK = VK_CULL_MODE_BACK_BIT,
		BACK_AND_FRONT = VK_CULL_MODE_FRONT_AND_BACK,
	};
	/**********************************************************/
	enum class POLYGON_MODE_ENUM : uint8_t {
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