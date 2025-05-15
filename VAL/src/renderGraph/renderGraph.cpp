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

#include <VAL/lib/renderGraph/renderGraph.hpp>
#include <VAL/lib/ext/streql.h>

#define PASS_BEGIN_KEYWORD "PASS_BEGIN"
#define PASS_END_KEYWORD "PASS_END"
#define READ_KEYWORD "READ("
#define WRITE_KEYWORD "WRITE("
#define READ_WRITE_KEYWORD "READ_WRITE("
#define INPUT_KEYWORD "INPUT("
#define FIXED_KEYWORD "FIXED"

typedef const char* KEYWORD;

namespace val {

	// discards a match is there is not a leading newline or space in front of it, 
	// if the match is at the beginning of the search, it will not be discarded.
	bool discardMatchIfNoLeadingSpaceOrNewline(const char* searchBegin, const char* matchBegin, const char* matchTarget) {
		if (matchBegin == searchBegin) {
			return false;
		}
		
		if (*(matchBegin - 1) == ' ') {
			return false;
		}

		if (*(matchBegin - 1) == '\n') {
			return false;
		}

		return true;
	}



	// searches for the first match of the target string as it scans backwards from beforeStr until it reaches cstrBegin.
	// if a match is found, it will return the distance from beforeStr to cstr begin,
	// otherwise it will return -1.
	int64_t findLastMatch(const char* cstrBegin, const char* beforeStr, const char* targ, const bool ignoreCommented = true) {
		const uint32_t targLen = strlen(targ);
		uint32_t i = 0u;

		while (beforeStr - i > cstrBegin) {
			if (ignoreCommented) {
				// Skip multi-line comments
				if (*(beforeStr - i) == '/' && *(beforeStr - i - 1) == '*') {
					i += 2; // Skip initial "*/"
					// Move to the start of the "/*"
					while (beforeStr - i > cstrBegin) {
						if (*(beforeStr - i) == '*' && *(beforeStr - i - 1) == '/') {
							i += 2;
							break;
						}
						i++;
					}
					continue;
				}
			}

			// Scan backwards for the target string
			bool match = true;
			for (uint32_t j = 0; j < targLen; ++j) {
				if (*(beforeStr - i - j) != targ[targLen - j - 1]) {
					match = false;
					break;
				}
			}

			if (match) {
				// first check to ensure that this line isn't commented out
				if (ignoreCommented) {
					uint32_t j = 0;
					while (j < UINT32_MAX)
					{
						if (beforeStr - i - j == cstrBegin || beforeStr - i - j - 1 == cstrBegin) {
							break;
						}

						if (*(beforeStr - i - j) == '/' && *(beforeStr - i - j - 1) == '/') {
							goto skip; // this line is commented out, skip it
						}

						j++;
					}
				}
				return i;
			}

		skip:
			i++;
		}

		return -1;
	}


	// looks for the next match of a target string.
	// it will return -1 if the target string is not found,
	// otherwise it will return the offset to the beginning
	// of the next occurance of target string
	// the discardMatchConditional is a function used to discard a match under a certain condition. It should return true to discard the match, false to keep the match
	//				it takes 3, c strings as args: (char* cstrbegin,  <- this is the beginning of the search, same as cstr
	//												char* matchBegin, <- this is where the beginning of the match is
	//												char* targ) <- this is the match target
	int64_t findNextMatch(const char* cstr, const char* targ, const bool ignoreCommented = true,
		uint32_t limit = UINT32_MAX, bool (*discardMatchConditional)(const char*, const char*, const char*) = NULL) {
		const uint32_t targlen = strlen(targ);
		uint8_t commented = 0; // 0 = not commented, 1 = single line comment, 2 = multiline comment
		for (uint16_t i = 0; i < limit; ++i) {
			if (cstr[i] == '\0') {
				return -1;
			}

			if (ignoreCommented) {
				if (i < limit - 1) {
					if ((cstr[i] == '/' && cstr[i + 1] == '/')) {
						commented = 1;
					}
					else if ((cstr[i] == '/' && cstr[i + 1] == '*'))
					{
						commented == 2;
					}
					if (commented == 2 && cstr[i] == '*' && cstr[i + 1] == '/') {
						commented = false;
						i += 2;
					}
					else if (commented==1 && cstr[i] == '\n')
					{
						commented = false;
					}
				}
			}

			if (!commented) {
				for (uint32_t j = 0; j <= targlen; ++j) {
					if (i + j > limit) {
						return -1;
					}
					if (j == targlen) {
						if (discardMatchConditional) {
							bool discard = discardMatchConditional(cstr, cstr + i, targ);
							if (discard) {
								break;
							}
						}
						return i + j;
					}
					if (cstr[i + j] != targ[j]) {
						break;
					}
				}
			}
		}
		return -1;
	}


	int64_t getClosingFigureOffset(const char* cstr, const char openingFig, const char closingFig, const bool ignoreComments = true) {
		uint32_t nestCount = 0u;
		uint32_t i = 0;
		while (true)
		{
			if (cstr[i] == '\0') { 
				return -1;
			}


			if (ignoreComments && cstr[i] == '/' && cstr[i + 1] == '/') {
				while (true) // for single line comments, continue until the next line
				{
					if (cstr[i] == '\0') {
						return -1;
					}

					if (cstr[i] == '\n') {
						break;
					}
					i++;
				}
				continue;
			}
			else if (ignoreComments && cstr[i] == '/' && cstr[i + 1] == '*') {
				while (true) // for multiline comments, continue until closing */
				{
					if (cstr[i] == '\0') {
						return -1; 
					}

					if (cstr[i] == '*' && cstr[i + 1] == '/') {
						break;
					}
					i++;
				}
				continue;
			}

			if (cstr[i] == openingFig) {
				nestCount++;
			}
			else if (cstr[i] == closingFig)
			{
				nestCount--;
				if (nestCount == 0) {
					return i;
				}
			}

			++i;
		}
		return -1;
	}

	// returns a pointer to the closing parenthesis. ')'
	// The cstr must be a pointer to the beginning parethensis '('
	// If it can't find a closing parenthesis, NULL will be returned
	const char* getClosingParenthesis(const char* cstr, const bool ignoreComments = true) {
		return cstr + getClosingFigureOffset(cstr, '(', ')', ignoreComments);
	}

	// returns a pointer to the closing bracket. '}'
	// The cstr must be a pointer to the beginning bracket '{'
	// If it can't find a closing bracket, NULL will be returned
	const char* getClosingBracket(const char* cstr, const bool ignoreComments = true) {
		return cstr + getClosingFigureOffset(cstr, '{', '}', ignoreComments);
	}


	// gets the end of a keyword.
	// i.e. Some keyswords end with a closing parenthesis, 
	// but others, like flags, don't end this way.
	// if it fails to find the end, a NULL pointer will be returned.
	const char* findKeywordEnd(KEYWORD keyword, const char* kywrdBegin) {
		if (keyword == PASS_BEGIN_KEYWORD) {
			return 1 + kywrdBegin + strlen(PASS_BEGIN_KEYWORD);
		}
		else if (keyword == PASS_END_KEYWORD) {
			return 1 + kywrdBegin + strlen(PASS_END_KEYWORD);
		}
		else if (keyword == FIXED_KEYWORD) {
			return 1 + kywrdBegin + strlen(FIXED_KEYWORD);
		}
		else if (keyword == READ_KEYWORD) {
			return 1 + getClosingParenthesis( -1 + kywrdBegin + strlen(READ_KEYWORD));
		}
		else if (keyword == WRITE_KEYWORD) {
			return 1 + getClosingParenthesis(-1 + kywrdBegin + strlen(WRITE_KEYWORD));
		}
		else if (keyword == READ_WRITE_KEYWORD) {
			return 1 + getClosingParenthesis(-1 + kywrdBegin + strlen(READ_WRITE_KEYWORD));
		}
		else if (keyword == INPUT_KEYWORD) {
			return 1 + getClosingParenthesis(-1 + kywrdBegin + strlen(INPUT_KEYWORD));
		}
	}


	// gets the offset to the beginning of the next line. I
	// If this is the last line, it will return -1;
	int64_t nextLine(const char* start) {
		int64_t i = 0;
		while (true)
		{
			if (start[i] == '\n') {
				++i;
				if (start[i] == '\0') {
					return -1;
				}
				if (start[i] == '\r') {
					++i;
				}
				if (start[i] == '\0') {
					return -1;
				}
				return i;
			}
			if (start[i] == '\0') {
				return -1;
			}
			i++;
		}
	}


	// the exec src opening bracket '{' should be directly after the last render pass keyword.
	// returns a pointer to the beginning of the exec src, just after the opening bracket.
	// If it fails, a NULL pointer will be returned.
	const char* findExecSrcBegin(const char* cstr) {
		const int64_t readOffset = findNextMatch(cstr, READ_KEYWORD);
		const int64_t writeOffset = findNextMatch(cstr, WRITE_KEYWORD);
		const int64_t readWriteOffset = findNextMatch(cstr, READ_WRITE_KEYWORD);
		const int64_t inputOffset = findNextMatch(cstr, INPUT_KEYWORD);
		const int64_t flag_fixedOffset = findNextMatch(cstr, FIXED_KEYWORD);

		KEYWORD lastKeyword = NULL;
		int64_t lastKeywordOffset = -1;
		int8_t keywordCount;

		// this algorithm works by checking for the first '{' outside of a keyword.
		
		uint64_t i = 0;
		while (true)
		{
			if (cstr[i] == '\0') {
				return NULL;
			}
			if (cstr[i] == '(') {
				const char* closeParenthesis = getClosingParenthesis(cstr+i);
				if (closeParenthesis == NULL) {
					return NULL; // failed to find closing parenthesis
				}
				i += uint64_t(closeParenthesis - cstr);
			}
			if (cstr[i] == '{') {
				// return 1 past {
				return cstr+i + 1;
			}
			i++;
		}
	}


	// note that the argsOut is a contigious block of memory, like so:
	// argsOuts = "arg1\0arg2\0"
	// the null terminators tell you how to seperate this data.
	void readArgs(const char* str, struct ARG_BLOCK* argBlock, const uint32_t charLimit) {
		// reads arguments seperated by ,
		// note that double commas ',,' will be treated as ','. This aligns with C standards.

		// arglist - a contigious block of memory divided by the seperator list
		char* arglist = NULL;
		uint16_t argCount = 0u;
		uint32_t arglistByteSize = 0u;

		uint32_t argclen=0u;
		for (uint32_t i = 0; i < charLimit; i++) {
			// ignore any leading or trailing spaces, but mind that spacing in-between words are important and must be kept

			if (str[i] == ',' || i == charLimit - 1) {
				if (argclen > 0) {

					// check for any trailing spaces,
					// wind back argclen until they 
					// are not considered as part of the arg
					uint16_t j = 0;
					while (str[i - j - 1]==' ') {
						j++;
						argclen--;
					}
					const uint32_t offset = i - j;
					// allocate argument and copy it
					arglist = (char*)realloc(arglist, arglistByteSize+argclen+1);
					memcpy(arglist+arglistByteSize, str + offset - argclen, argclen);
					// add null terminator
					arglist[arglistByteSize + argclen] = '\0';
					//printf("ARG: %s|\n", arglist + arglistByteSize);
					arglistByteSize += argclen + 1;

					argCount++;
				}
				argclen = 0;
			}
			else if (
				!(str[i] == ' ' && argclen == 0)/*ignore leading spaces*/) {
				argclen++;
			}
		}
		argBlock->args = arglist;
		argBlock->argCount = argCount;
	}

	VAL_RETURN_CODE RENDER_GRAPH::loadFromFile(const std::filesystem::path& filepath) {
		VAL_RETURN_CODE retCode = VAL_SUCCESS;
		srcFileName = filepath.string();

		FILE* fptr = NULL;
		fopen_s(&fptr, srcFileName.c_str(), "rb");
		if (fptr) {
			fseek(fptr, 0, SEEK_SET);
			uint64_t f_start = ftell(fptr);
			fseek(fptr, 0, SEEK_END);
			uint64_t f_end = ftell(fptr);
			// file length, in bytes
			const uint64_t f_len = f_end - f_start;

			fseek(fptr, 0, SEEK_SET); // set fptr back to the beginning
			

			srcFileContents = (char*)realloc(srcFileContents, f_len + 1);

			uint64_t i = 0u; // read file char by char
			while ((srcFileContents[i] = fgetc(fptr)) != EOF) {
				i++;
			}

			// null terminate and update length
			srcFileContents[f_len] = '\0';
			srcContentLen = f_len;

			if (fclose(fptr) == EOF) {
				printf("VAL: ERROR: Failed to close render graph source file\n");
				retCode = VAL_FAILURE;
			}
		}
		else {
			printf("VAL: ERROR: Failed to load render graph source file %s.\n", srcFileName.c_str());
			retCode = VAL_FAILURE;
		}

		return retCode;
	}


	VAL_RETURN_CODE RENDER_GRAPH::compile(SUPPORTED_COMPILER compiler, const COMPILE_ARGS& extraArgs /*DEFAULT = {}*/) {

	#ifndef NDEBUG
		if (strlen(srcFileContents)==0) {
			printf("VAL: ERROR: Failed to compile render graph, the src file has not been loaded. Perhaps you forgot to call loadFromFile?\n");

			return VAL_FAILURE;
		}
	#endif // !NDEBUG





		char* errorMsg;
		char* processed_src;
		uint64_t processed_src_len;
		const VAL_RETURN_CODE preprocess_res = preprocess(&processed_src, &processed_src_len, &errorMsg);
		if (preprocess_res == VAL_FAILURE) {
			cleanup();
			return VAL_FAILURE;
		}

		// the source file contents need to be placed in a temporary file after preprocessing.

		//using namespace val;
		//COMPILE_RETURN_CODE retcode = compileToDLL(srcPath, DLLname, compiler, "TESTLIB_EXPORTS",
		//	{ .cppStandard = CPP_20, .optimizationLvl = DISABLED });
		//if (retcode != COMPILE_SUCCESS) {
		//	printf("Compiliation failed");
		//	exit(EXIT_FAILURE);
		//}

		//void(__cdecl * printStr)() = VAL_loadDLLfunction("dlls/testDLL.dll", "printStr");

		//printStr();

		return VAL_SUCCESS;
	}

	void RENDER_GRAPH::cleanup() {
		if (srcFileContents) {
			free(srcFileContents);
			srcFileContents = NULL;
			srcFileContentsLen = 0u;
		}
		if (preprocessFileName) {
			free(preprocessFileName);
			preprocessFileName = NULL;
		}
	}

	VAL_RETURN_CODE RENDER_GRAPH::readPass(struct PASS_INFO* __passInfo__, char* passBegin, const uint32_t* passStrLen, char** __error__) {
		const char* error = *(__error__);
#ifndef NDEBUG
		if (!__passInfo__ || !passBegin) {
			error = "Failed to read pass, invalid missing arguments!";
			return VAL_FAILURE;
		}
#endif // !NDEBUG

		PASS_INFO& passInfo = *__passInfo__;

		char* cur = 0;
		// start reading at pass begin, first get name
		{
			uint8_t i = 0;

			// check for PASS_BEGIN keyword
			if (strncmp(passBegin, PASS_BEGIN_KEYWORD, strlen(PASS_BEGIN_KEYWORD)) != 0) {
				error = "PASS_BEGIN is missing";
				return VAL_FAILURE;
			}
			
			cur = passBegin + strlen(PASS_BEGIN_KEYWORD);

			uint8_t passNameLen = 0;
			// now get the name length
			for (uint8_t i = 0; i < UINT8_MAX; ++i) {
				if (*(cur + i) == ')') {
					passNameLen = i;
					break;
				}
				if (i == UINT8_MAX - 1) {
					error = "Pass name exceeds max length of 255 characters, or closure ')' is missing!";
					return VAL_FAILURE;
				}
			}

			// alloc pass name and add null terminator
			passInfo.passName = (char*)realloc(passInfo.passName, passNameLen + 1);
			passInfo.passName[passNameLen] = '\0';

			// copy pass name into allocated str
			memcpy(passInfo.passName, cur, passNameLen);
		}

		// get reads
		{
			// search for "READ(", the -1 is to account for the fact that getClosingParenthesis must start on '('
			const int64_t nextMatchOffset = findNextMatch(cur, READ_KEYWORD);
			char* matchOffsetBegin = -1 + cur + nextMatchOffset;
			if (nextMatchOffset >= 0) { // -1 or less means no reads
				cur = matchOffsetBegin;
				// search for closing ')'
				char* matchOffsetEnd = (char*)getClosingParenthesis(cur);
				if (matchOffsetEnd == NULL) {
					error = "Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.readBlock, matchOffsetEnd - matchOffsetBegin);

				// set cur to closing ")"
				cur = matchOffsetEnd;
			}
		}

		//cur += nextLine(cur);

		// get writes
		{
			// search for "WRITE(" the -1 is to account for the fact that getClosingParenthesis must start on '('
			const int64_t nextMatchOffset = findNextMatch(cur, WRITE_KEYWORD);
			char* matchOffsetBegin = -1 + cur + nextMatchOffset;
			if (nextMatchOffset >= 0) { // -1 or less means no writes
				cur = matchOffsetBegin;
				// search for closing ')'
				char* matchOffsetEnd = (char*)getClosingParenthesis(cur);
				
				if (matchOffsetEnd == NULL) {
					error = "Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.writeBlock, matchOffsetEnd - matchOffsetBegin);

				// set cur to closing ")"
				cur = matchOffsetEnd;
			}
		}

		// get read writes
		{
			//extractPassData(&passInfo, READ_WRITE);

		}

		// get flags (if any)
		{
			//extractPassData(&passInfo, FLAGS);
		}
		// get args
		{
			//extractPassData(&passInfo, ARGS);
		}

		int64_t endOffset = 0u;
		// check for end
		{
			endOffset = findNextMatch(cur, PASS_END_KEYWORD);
			if (endOffset == -1) {
				error = "PASS_BEGIN is missing END_PASS";
				return VAL_FAILURE;
			}
		}

		// get exec src
		{
			// the exec src opening bracket '{' should be directly after the last keyword.
			const char* execBegin = findExecSrcBegin(passBegin);
			const char* execBeginBracket = execBegin - 1;

			const char* execClosingBracket = getClosingBracket(execBeginBracket);

			if (execBegin == NULL || execBeginBracket == NULL || execClosingBracket == NULL) {
				error = "Error parsing pass exec, possible missing '(' or ')'";
				// failed to find exec src
				return VAL_FAILURE;
			}


			passInfo.execSrc = (char*)execBegin;
			passInfo.execSrcLen = uint32_t(execClosingBracket - execBeginBracket - 2);

			printf("-- %.*s --\n", passInfo.execSrcLen, passInfo.execSrc);

			//extractPassData(&passInfo, &EXEC_SRC);
		}


		return VAL_SUCCESS;
	}


	VAL_RETURN_CODE RENDER_GRAPH::preprocess(char** processed_src, uint64_t* processed_src_len, char** errorMsg)
	{
		char* src = srcFileContents;

		if (!src || !(*processed_src) || !processed_src_len || !(*errorMsg)) {
			printf("VAL: ERROR: Failed to preprocess file, invalid arguments!\n");
			return VAL_FAILURE;
		}

		// Passes begin with PASS_BEGIN(NAME), and end with PASS_END.
		// They can have 5 different arguments: 
		// READ, <- Graphics resources that it will read from
		// WRITE, <- Graphics resources that it will write to
		// READ_WRITE, <- Graphics resources that it will read and write from
		// FIXED <- a flag to specificy to "bake" the command buffers for this pass, in other words they will only be updated once and never reset
		// INPUT <- Additional non-graphics variables, i.e. pipeline handles, numeric values, etc

		// A very basic render pass might look something like this:
		/*
		PASS_BEGIN(DRAW_RECT)
		READ(buffer& vertexBuffer, buffer& indexBuffer)
		WRITE(NULL)
		READ_WRITE(NULL)
		FIXED
		INPUT(graphicsPipelineHdl& pipeline, window& wind)
		{
			// execute pass
		}
		PASS_END
		*/

		const char* passBeginKeyword = "PASS_BEGIN";
		uint32_t cur = 0u; // index of the char that is being scanned
		while (cur < srcContentLen)
		{
			// look for PASS_BEGIN keyword
			if (strneql(src + cur, passBeginKeyword, strlen(passBeginKeyword))) {
				PASS_INFO info{};
				// this marks the length between the beginning (first char)
				// and the end (last char) of the pass src.
				uint32_t jmpLen = 0u;
				char* readerr = NULL;
				if (VAL_FAILURE==readPass(&info, src + cur, &jmpLen, &readerr)) {
					printf("VAL: Error preprocessing render pass: %s\n", readerr);
					return VAL_FAILURE;
				}
				

				// if the pass was read successfully, write it to the new src file

				// note that the compiled src file should use (void PASS_MAIN()) as the main execution



			}

			cur++;
		}



		return VAL_SUCCESS;
	}
}