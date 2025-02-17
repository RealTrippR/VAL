#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void UBO_Handle::update(VAL_PROC& proc, void* data) {
		memcpy(proc._uniformBuffersMapped[proc._currentFrame][_index],data,_size);
	}
}