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
#include <format>

#define PASS_BEGIN_KEYWORD "PASS_BEGIN"
#define PASS_END_KEYWORD "PASS_END"
#define READ_KEYWORD "READ"
#define WRITE_KEYWORD "WRITE"
#define READ_WRITE_KEYWORD "READ_WRITE"
#define INPUT_KEYWORD "INPUT"
#define FIXED_KEYWORD "FIXED"

typedef const char* KEYWORD;

namespace val {

	std::string removeParentDirs(const std::string& fullPath) {
		size_t pos = fullPath.find_last_of("/\\"); // find last '/' or '\'
		if (pos == std::string::npos)
			return fullPath; // no directory separators found, return full string
		return fullPath.substr(pos + 1);
	}

	std::string removeAllFileExtensions(const std::string& filename) {
		size_t dotPos = filename.find('.');
		if (dotPos == std::string::npos) {
			return filename; // No extension found
		}
		return filename.substr(0, dotPos);
	}

	bool isFilenameInUseOrLocked(const char* filename) {
		FILE* fptr = NULL;
		errno_t err = fopen_s(&fptr, filename, "rb");
		bool retval = true;

		if (fptr == NULL) {
			retval = false;
		}
		else {
			fclose(fptr);
		}
		// file exists, but is locked
		if (err == EACCES) {
			return true;
		}

		return retval;
	}

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
	// it will return NULL if the target string is not found,
	// otherwise it will return the offset to the first char (beginning)
	// of the next occurance of target string
	// the discardMatchConditional is a function used to discard a match under a certain condition. It should return true to discard the match, false to keep the match
	//				it takes 3, c strings as args: (char* cstrbegin,  <- this is the beginning of the search, same as cstr
	//												char* matchBegin, <- this is where the beginning of the match is
	//												char* targ) <- this is the match target
	char* findNextMatch(const char* cstr, const char* targ, const bool ignoreCommented = true,
		uint32_t limit = UINT32_MAX, bool (*discardMatchConditional)(const char*, const char*, const char*) = NULL) {
		const uint32_t targlen = strlen(targ);
		COMMENT_TYPE comment = NONE;
		for (uint16_t i = 0; i < limit; ++i) {
			if (cstr[i] == '\0') {
				return NULL;
			}

			if (ignoreCommented) {
				if (comment == NONE && cstr[i] == '/' && cstr[i + 1] == '/') {
					// it's a single line comment, ignore
					comment = SINGLE_LINE;
				}
				else if (comment == SINGLE_LINE && cstr[i] == '\n') {
					comment = NONE;
				}
				else if (comment == NONE && cstr[i] == '/' && cstr[i + 1] == '*') {
					comment = MULTI_LINE;
				}
				else if (comment == MULTI_LINE && cstr[i] == '*' && cstr[i + 1] == '/') {
					comment = NONE;
				}
			}

			if (comment == NONE) {
				for (uint32_t j = 0; j <= targlen; ++j) {
					if (i + j > limit) {
						return NULL;
					}
					if (j == targlen) {
						if (discardMatchConditional) {
							bool discard = discardMatchConditional(cstr, cstr + i, targ);
							if (discard) {
								break;
							}
						}
						return (char*)cstr + i + j - strlen(targ);
					}
					if (cstr[i + j] != targ[j]) {
						break;
					}
				}
			}
		}
		return NULL;
	}

	// indentical to findNextMatch, 
	// except it will return a pointer to the character just after
	// the of the next occurance of the target string.
	char* findNextMatchAdditive(const char* cstr, const char* targ, const bool ignoreCommented = true,
		uint32_t limit = UINT32_MAX, bool (*discardMatchConditional)(const char*, const char*, const char*) = NULL) {
		return findNextMatch(cstr, targ, ignoreCommented, limit, discardMatchConditional) + strlen(targ);
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
		if (cstr[0] != '(') {
			throw std::exception("The cstr of getClosingParenthesis must be a pointer to '('");
		}
		return cstr + getClosingFigureOffset(cstr, '(', ')', ignoreComments);
	}

	// returns a pointer to the closing bracket. '}'
	// The cstr must be a pointer to the beginning bracket '{'
	// If it can't find a closing bracket, NULL will be returned
	const char* getClosingBracket(const char* cstr, const bool ignoreComments = true) {
		if (cstr[0] != '{') {
			throw std::exception("The cstr of getClosingParenthesis must be a pointer to '{'");
		}
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
		// this algorithm works by checking for the first '{' outside of a keyword.
		// it works by jumping to the next ')' every time it detects a '(', thus skipping keywords.
		
		// currently, it does not check for comments, this must be fixed.
		uint64_t i = 0;
		COMMENT_TYPE comment = NONE;
		while (true)
		{
			if (cstr[i] == '\0') {
				return NULL;
			}
			else if (comment == NONE && cstr[i] == '/' && cstr[i + 1] == '/') {
				// it's a single line comment, ignore
				comment = SINGLE_LINE;
			} 
			else if (comment == SINGLE_LINE && cstr[i] == '\n') {
				comment = NONE;
			}
			else if (comment == NONE && cstr[i] == '/' && cstr[i + 1] == '*') {
				comment = MULTI_LINE;
			}
			else if (comment == MULTI_LINE && cstr[i] == '*' && cstr[i + 1] == '/') {
				comment = NONE;
			}

			if (comment == NONE) {
				if (cstr[i] == '(') {
					const char* closeParenthesis = getClosingParenthesis(cstr + i);
					if (closeParenthesis == NULL) {
						return NULL; // failed to find closing parenthesis
					}
					// jump to the closing parenthesis
					i += uint64_t(closeParenthesis - cstr);
				}
				if (cstr[i] == '{') {
					// return 1 past {
					return cstr + i + 1;
				}
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

		if (str[0] == '(') {
			throw std::exception("The str argument in readArgs must NOT begin with '('");
		}

		// arglist - a contigious block of memory divided by the seperator list
		char* arglist = NULL;
		uint16_t argCount = 0u;
		uint32_t arglistByteSize = 0u;

		uint32_t argclen=0u;
		for (uint32_t i = 0; i < charLimit; i++) {
			// ignore any leading or trailing spaces, but mind that spacing in-between words are important and must be kept
			const char tmp = str[i];
			if (str[i] == ',' || i == charLimit - 1) {

				if (argclen > 0) {

					// check for any trailing spaces,
					// wind back argclen until they 
					// are not considered as part of the arg
					uint16_t j = 0;
					while (str[i - j]==' ') {
						j++;
						argclen--;
					}
					const uint32_t offset = i - j;
					// allocate argument and copy it
					arglist = (char*)realloc(arglist, arglistByteSize+argclen+1);
					memcpy(arglist+arglistByteSize, str + offset - argclen, argclen);
					const char* tmp = str + offset - argclen;
					
					// add null terminator
					arglist[arglistByteSize + argclen] = '\0';
					//printf("ARG: %s|\n", arglist + arglistByteSize);
					arglistByteSize += argclen + 1;

					argCount++;
				}
				argclen = 0;

				i++; //increment i to after ','
			}
			else if (!(str[i] == ' ' && argclen == 0)/*ignore leading spaces*/) {
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



	VAL_RETURN_CODE RENDER_GRAPH::compile(SUPPORTED_COMPILER compiler, const string& DLL_file_name, filepath compileToDir = "", const COMPILE_ARGS& extraArgs /*DEFAULT = {}*/) {

		if (strlen(srcFileContents)==0) {
			printf("VAL: ERROR: Failed to compile render graph, the src file has not been loaded. Perhaps you forgot to call loadFromFile?\n");

			return VAL_FAILURE;
		}
		if (!compileToDir.empty()) {
			std::string pathStr = compileToDir.string();
			std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
			compileToDir = pathStr;
		}



		char* errorMsg;
		string processed_src;
		const VAL_RETURN_CODE preprocess_res = preprocess(&processed_src, &errorMsg);
		if (preprocess_res == VAL_FAILURE) {
			cleanup();
			return VAL_FAILURE;
		}


		string srcFilepathNO_EXT = removeAllFileExtensions(srcFileName);
		string srcFilepathNO_EXT_NO_PARENT_DIRS = removeParentDirs(srcFilepathNO_EXT);
		string processedSrcFileName = compileToDir.string() + "/" + srcFilepathNO_EXT_NO_PARENT_DIRS;
		processedSrcFileName.append("__processed").append(".cpp");

		string processedHeaderFileName = compileToDir.string() + "/" + srcFilepathNO_EXT_NO_PARENT_DIRS;
		processedHeaderFileName.append("__processed").append(".h");

								/* before you go and make the same mistake again,
								* note that adding srcFileNameNO_EXT before
								* DLL_EXPORT_DEFINE or DLL_API_DEFINE is unsafe,
								* because the srcFileNameNO_EXT can include
								* parent directories of that file. For example:
								* myDir/mysrc
								*/
		string DLL_EXPORT_DEFINE = "VAL_RENDERGRAPH_EXPORTS";
		string DLL_API_DEFINE = "VAL_RENDERGRAPH_API";

		// insert the header include at the beginning of the dll src for proper compilation
		processed_src.insert(0, "#include \"" + removeParentDirs(processedHeaderFileName) + "\"\n");

		{
			// the source file contents need to be placed in a temporary file after preprocessing.
			FILE* processedSrcFptr = NULL;
			errno_t openErr = fopen_s(&processedSrcFptr, processedSrcFileName.c_str(), "w");
			if (openErr == EEXIST || processedSrcFptr == NULL) {
				if (processedSrcFptr) {
					fclose(processedSrcFptr);
				}
				printf("VAL:ERROR: Failed to open processed header file for writing, this file already exists or is locked! %s\n", processedSrcFileName.c_str());
				return VAL_FAILURE;
			}

			if (processed_src.length() > 0) {
				fwrite(processed_src.c_str(), processed_src.length(), 1, processedSrcFptr);
			}
			else {
				printf("Nothing to compile, the processed src length is 0!\n");
				return VAL_FAILURE;
			}
			fclose(processedSrcFptr);
		}
		{
			// create the header file, which is needed to tell the compiler how to properly link it

			FILE* headerFptr = NULL;
			errno_t openErr = fopen_s(&headerFptr, processedHeaderFileName.c_str(), "w");
			if (openErr == EEXIST || headerFptr == NULL) {
				if (headerFptr) {
					fclose(headerFptr);
				}
				printf("VAL:ERROR: Failed to open processed header file for writing, this file already exists or is locked! %s\n", processedHeaderFileName.c_str());
				return VAL_FAILURE;
			}

			const string headerSrc =
				std::format(
					"#include \"string.h\"\n"
					"#ifdef {}\n"
					"#define {} __declspec(dllexport)\n"
					"#else\n"
					"#define {} __declspec(dllimport)\n"
					"#endif\n"
					/*extern C is to stop C++ namespace mangling, which will break the DLL loader*/
					"extern \"C\" {} void pass_main();\n",
					DLL_EXPORT_DEFINE,
					DLL_API_DEFINE,
					DLL_API_DEFINE,
					DLL_API_DEFINE
				);

			int tmp = headerSrc.size();
			fwrite(headerSrc.c_str(), headerSrc.size(), 1, headerFptr);
			fclose(headerFptr);

		}




		using namespace val;
		const string dll_path = compileToDir.string() + "/" + removeAllFileExtensions(DLL_file_name);
		COMPILE_RETURN_CODE retcode = compileToDLL(processedSrcFileName, dll_path, compiler, DLL_EXPORT_DEFINE, extraArgs);
		if (retcode != COMPILE_SUCCESS) {
			printf("VAL: ERRROR: Compiliation failed\n");
			return VAL_FAILURE;
		}

		passMain = VAL_loadDLLfunction(dll_path.c_str(), "pass_main");
		if (!passMain) {
			printf("VAL: ERROR: Failed to load pass_main from dll!\n");
			return VAL_FAILURE;
		}
		return VAL_SUCCESS;
	}

	void RENDER_GRAPH::nextFrame() {
		passMain();
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

	VAL_RETURN_CODE RENDER_GRAPH::readPass(struct PASS_INFO* __passInfo__, char* passBegin, uint32_t* passStrLen, char** __error__) {
		const char* error = *(__error__);
#ifndef NDEBUG
		if (!__passInfo__ || !passBegin) {
			error = "Failed to read pass, missing arguments!";
			return VAL_FAILURE;
		}
#endif // !NDEBUG

		PASS_INFO& passInfo = *__passInfo__;

		char* cur = passBegin;
		// start reading at pass begin, first get name
		{
			// move to after PassBegin
			cur = findNextMatchAdditive(passBegin, PASS_BEGIN_KEYWORD);


			const char* nameOpeningParen = findNextMatch(cur, "(");
			if (nameOpeningParen == NULL) {
				error = "Name opening parenthesis '(' is missing";
				return VAL_FAILURE;
			}
			const char* nameClosingBracket = getClosingParenthesis(nameOpeningParen);
			if (nameClosingBracket == NULL) {
				error = "Name closing parenthesis ')' is missing";
				return VAL_FAILURE;
			}
			uint8_t passNameLen = nameClosingBracket - nameOpeningParen - 1;
		
			// alloc pass name
			passInfo.passName = (char*)realloc(passInfo.passName, passNameLen + 1);
			if (passInfo.passName == NULL) {
				error = "Out of memory for pass name!";
				return VAL_FAILURE;
			}
			
			// null terminate
			passInfo.passName[passNameLen] = '\0';
			// copy pass name into allocated str
			memcpy(passInfo.passName, nameOpeningParen+1, passNameLen);

			// set cur to just past the name closing parenthesis
			cur = (char*)nameClosingBracket + 1;
		}

		// get reads
		{
			// search for "READ("
			KEYWORD readKeyword = findNextMatchAdditive(cur, READ_KEYWORD);
			if (readKeyword) {
				char* argsBeginParenthesis = (char*)readKeyword;
				cur = argsBeginParenthesis + 1;
				// search for closing ')'
				char* argsEndParenthesis = (char*)getClosingParenthesis(argsBeginParenthesis);
				if (argsEndParenthesis == NULL) {
					error = "Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.readBlock, argsEndParenthesis - argsBeginParenthesis);
				

				PRINT_ARG_BLOCK(&passInfo.readBlock);
				// set cur to closing ")"
				cur = argsEndParenthesis;
			}
		}

		//cur += nextLine(cur);

		// get writes
		{
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

		KEYWORD passEnd = NULL;
		// check for end
		{
			passEnd = findNextMatch(cur, PASS_END_KEYWORD);
			if (passEnd == NULL) {
				error = "PASS_BEGIN is missing END_PASS";
				return VAL_FAILURE;
			}

			// set pass string length
			*passStrLen = passEnd - passBegin;
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


			if (passInfo.execSrc) {
				free(passInfo.execSrc);
			}
			passInfo.execSrcLen = uint32_t(execClosingBracket - execBeginBracket - 2);
			// passInfo must own a copy of exec begin, as per the standards.
			passInfo.execSrc = (char*)malloc(passInfo.execSrcLen);
			if (passInfo.execSrc) {
				memcpy(passInfo.execSrc, execBeginBracket + 1, passInfo.execSrcLen);
			}
			else {
				error = "Out of system memory, could not allocate memory for passInfo.execSrc";
				return VAL_FAILURE;
			}

			printf("-- %.*s --\n", passInfo.execSrcLen, passInfo.execSrc);

			//extractPassData(&passInfo, &EXEC_SRC);
		}


		return VAL_SUCCESS;
	}


	VAL_RETURN_CODE RENDER_GRAPH::preprocess(string* processed_src_out, char** errorMsg)
	{
		char* src = srcFileContents;

		if (!src || !processed_src_out || !(*errorMsg)) {
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

		string& processedSrc = *processed_src_out;


		PASS_INFO* passInfos = NULL;
		uint16_t passInfoCount = 0u;
		
		const char* passBeginKeyword = "PASS_BEGIN";
		uint32_t cur = 0u; // index of the char that is being scanned
		while (cur < srcContentLen)
		{
			// look for PASS_BEGIN keyword
			if (strneql(src + cur, passBeginKeyword, strlen(passBeginKeyword))) {
				passInfoCount++;
				PASS_INFO* tmpPassInfos = (PASS_INFO*)realloc(passInfos, passInfoCount * sizeof(PASS_INFO));

				// realloc failed
				if (tmpPassInfos == NULL) {
					passInfoCount--;
					goto bail;
				}

				passInfos = tmpPassInfos;

				PASS_INFO* curPassInfo = &passInfos[passInfoCount - 1];
				memset(curPassInfo, 0, sizeof(PASS_INFO)); // 0 init pass info

				// this marks the length between the beginning (first char)
				// and the end (last char) of the pass src.
				uint32_t jmpLen = 0u;
				char* readerr = NULL;
				if (VAL_FAILURE==readPass(curPassInfo, src + cur, &jmpLen, &readerr)) {
					printf("VAL: Error preprocessing render pass: %s\n", readerr);

					goto bail;

					return VAL_FAILURE;
				}
				
				PRINT_ARG_BLOCK(&(curPassInfo->readBlock));

			}

			cur++;
		}


		processedSrc.append("#include <stdio.h>\n");

		processedSrc.append("void pass_main() {\n");

		processedSrc.append("printf(\"Hello from pass_main()!\");\n");
		// all passes have all been read sucessfully, write to the src
		// note that the compiled src file should use (void pass_main()) as the entry point
		for (uint16_t i = 0u; i < passInfoCount; ++i) {

		}

		processedSrc.append("\n}");

		
	bail:
		if (passInfos) {
			for (uint16_t i = 0u; i < passInfoCount; ++i) {
				PASS_INFO_CLEANUP(&(passInfos[i]));
			}
			free(passInfos);
		}

		return VAL_SUCCESS;
	}
}