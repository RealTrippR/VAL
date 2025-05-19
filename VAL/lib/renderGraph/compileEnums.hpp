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

#ifndef VAL_COMPILE_ENUMS_H
#define VAL_COMPILE_ENUMS_H
#include <string>

namespace val {
	enum COMPILE_RETURN_CODE {
		COMPILE_SUCCESS,
		COMPILE_FAILURE_SRC_FILE_INVALID,
		COMPILE_FAILURE_UNSUPPORTED_COMPILER,
		COMPILE_FAILURE_CMD_FAIL,
		COMPILE_FAILURE_INVALID_CPP_VER
	};

	enum CPP_STANDARD {
		CPP_14,
		CPP_17,
		CPP_20,
		CPP_23,
		CPP_DRAFT,
		INVALID_VER
	};

	enum SUPPORTED_COMPILER {
		MSVC,
		GPLUSPLUS,
		CLANG
	};

	enum OPTIMIZATION_LEVEL {
		DISABLED,
		O1,
		O2,
		Ox
	};

	enum COMMENT_TYPE {
		NONE,
		SINGLE_LINE,
		MULTI_LINE
	};


	/*****************************************/
	/* Function declarations */

	const std::string CPP_StandardToString(const CPP_STANDARD& cppStd);



	/*****************************************/
}

#endif // !VAL_COMPILE_ENUMS_H