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

//#ifdef VAL_ENABLE_EXPIREMENTAL

#ifndef VAL_RENDER_GRAPH_H
#define VAL_RENDER_GRAPH_H

#include <VAL/lib/renderGraph/passInfo.h>
#include <VAL/lib/renderGraph/renderGraphBlock.h>
#include <VAL/lib/renderGraph/DLL_Compiler.hpp>
#include <VAL/lib/renderGraph/loadDLL.h>

#include <VAL/lib/renderGraph/compileArgs.hpp>
#include <filesystem>

namespace val {

	class RENDER_GRAPH {
	public:
		RENDER_GRAPH() {
			VAL_incDLLloaderRefCount();
		}
		~RENDER_GRAPH() {
			VAL_decDLLloaderRefCount();
			cleanup();
		}
	public:
		VAL_RETURN_CODE loadFromFile(const std::filesystem::path& srcPath);

		VAL_RETURN_CODE compile(SUPPORTED_COMPILER compiler, const string& DLL_file_name, filepath compileToDir, const COMPILE_ARGS& extraArgs = {});

		void nextFrame();

		void(__cdecl* get_pass_main())() {
			return passMain;
		}

	private:
		void cleanup();

		VAL_RETURN_CODE readPass(struct PASS_INFO* __passInfo__, char* passBegin, uint32_t* passStrLen, char** error);

		VAL_RETURN_CODE preprocess(string* processed_src_out, char** errorMsg);

	private:

		char* preprocessFileName = NULL;
		char* srcFileContents = NULL;
		uint32_t srcContentLen = 0u;
		std::string srcFileName;
		uint64_t srcFileContentsLen = 0u;

		void(__cdecl* passMain)();
	};
}


#endif // !VAL_RENDER_GRAPH_H

//#endif // !VAL_ENABLE_EXPIREMENTAL
