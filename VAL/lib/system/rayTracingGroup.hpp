#ifndef VAL_RAYTRACING_GROUP_HPP
#define VAL_RAYTRACING_GROUP_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/raytracingEnums.hpp>
namespace val
{
	class Shader;
	class RaytracingGroup
	{
	public:
		RAYGROUP group;
		tiny_vector<Shader*> shaders;

		// debug only, will print warning or error messages if something is wrong
		void dbgValidateRaygroup() const;
	};
}

#endif // !VAL_RAYTRACING_GROUP_HPP