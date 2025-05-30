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

// EXPERIMENTAL
#ifdef VAL_ENABLE_EXPIREMENTAL

#ifndef VAL_RENDER_GRAPH_H
#define VAL_RENDER_GRAPH_H

#include <VAL/lib/VALreturnCode.h>
#include <VAL/lib/renderGraph/graphEnums.hpp>
#include <VAL/lib/renderGraph/passInfo.h>
#include <VAL/lib/renderGraph/renderGraphBlock.h>

#include <VAL/lib/renderGraph/argHandleList.hpp>
#include <filesystem>
#include <unordered_map>
#include <cstdarg>

#include <VAL/lib/renderGraph/pass.hpp>
#include <VAL/lib/renderGraph/renderPassBeginType.hpp>

#define __CONCAT2__(a, b) a##b
#define __CONCAT__(a, b) __CONCAT2__(a, b)

#define __STR2__(x) #x
#define __STR__(x) __STR2__(x)

#ifdef VAL_RENDER_PASS_COMPILE_MODE
#define GRAPH_FILE(...) <VAL\lib\renderGraph\blank.h>
#else
#define GRAPH_FILE(x) __STR__(__CONCAT__(x, __processed.hpp))
#endif


namespace val {

	using std::string;
	typedef std::filesystem::path filepath;

	class RENDER_GRAPH {
	public:
		~RENDER_GRAPH() {
			cleanup();
		}
	public:

		VAL_RETURN_CODE loadFromFile(const std::filesystem::path& srcPath);

		VAL_RETURN_CODE compile(const uint8_t framesInFlight, const filepath& compileToDir = "");

	private:
		void cleanup();

		VAL_RETURN_CODE readPass(struct PASS_INFO* __passInfo__, char* passBegin, uint32_t* passStrLen, char** error);

		VAL_RETURN_CODE preprocess(string* processed_src_out, char** errorMsg, const uint8_t framesInFlight);

	private:
		char* preprocessFileName = NULL;
		char* srcFileContents = NULL;
		uint32_t srcContentLen = 0u;
		std::string srcFileName;
		uint64_t srcFileContentsLen = 0u;
	};
}


#endif // !VAL_RENDER_GRAPH_H

#endif // !VAL_ENABLE_EXPIREMENTAL