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

#ifndef VAL_COMPILE_ARGS_HPP
#define VAL_COMPILE_ARGS_HPP

#include <vector>
#include <VAL/lib/renderGraph/compileEnums.hpp>
#include <string>

namespace val {
	using std::string;

	struct COMPILE_INCLUDE_INFO {
		std::vector<string> includeDirs;
		std::vector<string> includeFiles;
	};
	struct COMPILE_LINKER_INFO {
		std::vector<string> linkDirs; /*Search dirs for libs*/
		std::vector<string> linkLibs;
	};
	struct COMPILE_ARGS {
		std::vector<string> additionalCmdLineArguments;
		std::vector<string> preprocessorDefinitions;
		CPP_STANDARD cppStandard = CPP_20;
#ifndef NDEBUG
		OPTIMIZATION_LEVEL optimizationLvl = DISABLED;
#else 
		OPTIMIZATION_LEVEL optimizationLvl = O2;
#endif // !NDEBUG
		COMPILE_INCLUDE_INFO includeInfo{};
		COMPILE_LINKER_INFO linkerInfo{};
	};
}

#endif // !VAL_COMPILE_ARGS_HPP