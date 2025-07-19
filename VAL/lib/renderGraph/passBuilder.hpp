#ifndef VAL_RENDER_GRAPH_PASS_BUILDER_HPP
#define VAL_RENDER_GRAPH_PASS_BUILDER_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/descriptorSheets/descriptorSheet.hpp>

namespace val {
	class PassBuilder {
	public:
		inline void read(const ObjectDescriptorInfo& read) {
			reads.push_back(read);}
		inline void write(const ObjectDescriptorInfo& write) {
			writes.push_back(write);}

		tiny_vector<ObjectDescriptorInfo> reads;
		tiny_vector<ObjectDescriptorInfo> writes;
	};
}

#endif // !VAL_RENDER_GRAPH_PASS_BUILDER_HPP