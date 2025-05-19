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

#ifndef VAL_RENDER_GRAPH_DLL_COMPILER_H
#define VAL_RENDER_GRAPH_DLL_COMPILER_H

#include "compileArgs.hpp"
#include "VALreturnCode.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

namespace val {
	using filepath = std::filesystem::path;

	COMPILE_RETURN_CODE compileToDLL(filepath srcPath, string DLLname, SUPPORTED_COMPILER compiler, const std::string EXPORT_FLAG, const COMPILE_ARGS& extraArgs = {});
}

#endif /*!VAL_RENDER_GRAPH_DLL_COMPILER_H*/