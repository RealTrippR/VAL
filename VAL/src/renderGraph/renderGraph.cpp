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
#define VAL_ENABLE_EXPIREMENTAL


#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/renderGraph/renderGraph.hpp>
#include <VAL/lib/ext/streql.h>
#include <format>
#include <regex>

#define PASS_BEGIN_KEYWORD "PASS_BEGIN"
#define PASS_END_KEYWORD "PASS_END"
#define READ_KEYWORD "READ"
#define WRITE_KEYWORD "WRITE"
#define READ_WRITE_KEYWORD "READ_WRITE"
#define INPUT_KEYWORD "INPUT"
#define FIXED_BEGIN_KEYWORD "FIXED_BEGIN"
#define FIXED_END_KEYWORD "FIXED_END"

#define ARR_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
typedef const char* KEYWORD;

namespace val {

	bool isCharBlank(const char c) {
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0') {
			return true;
		}
		return false;
	}

	string getCurrentFrameIndexArgName() {
		return "__current_frame_index__";
	}

	void removeTrailingComma(string& processedSrc) 
	{
		// Start from the end and skip whitespace
		size_t i = processedSrc.size();
		while (i > 0 && std::isspace(static_cast<unsigned char>(processedSrc[i - 1]))) {
			--i;
		}

		// If the last non-whitespace character is a comma, remove it and following whitespace
		if (i > 0 && processedSrc[i - 1] == ',') {
			processedSrc.erase(i - 1);
		}
	}

	string argBlockToString(const ARG_BLOCK& argblock) 
	                 {
		string str;
		for (uint16_t i = 0; i < argblock.argCount; ++i) {
			const string tmp = string(GET_ARG_FROM_ARG_BLOCK(&argblock, i));
			str.append(tmp);
			str.append(", ");
		}
		return str;
	}

	void handleComment(COMMENT_TYPE* curCommentType, const char* curStrPos) {
		if (*curCommentType == NONE && curStrPos[0] == '/' && curStrPos[1] == '/') {
			// it's a single line comment, ignore
			*curCommentType = SINGLE_LINE;
		}
		else if (*curCommentType == SINGLE_LINE && curStrPos[0] == '\n') {
			*curCommentType = NONE;
		}
		else if (*curCommentType == NONE && curStrPos[0] == '/' && curStrPos[1] == '*') {
			*curCommentType = MULTI_LINE;
		}
		else if (*curCommentType == MULTI_LINE && curStrPos[0] == '*' && curStrPos[1] == '/') {
			*curCommentType = NONE;
		}
	}

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

		if (*(matchBegin - 1) == '\t') {
			return false;
		}

		if (*(matchBegin - 1) == '\r') {
			return false;
		}

		if (*(matchBegin - 1) == '\n') {
			return false;
		}

		return true;
	}
	
	bool discardMatchIfNoLeadingBlankAndNoTrailingBlank(const char* searchBegin, const char* matchBegin, const char* matchTarget) {
		bool leadDisc = discardMatchIfNoLeadingSpaceOrNewline(searchBegin, matchBegin, matchTarget);
		bool trailDisc = false;

		if (isCharBlank(*(matchBegin + strlen(matchTarget))) == false) {
			trailDisc = true;
		}
		return leadDisc || trailDisc;
	}

	// ignores newlines, tabs, and spaces
	bool ignoreCharactersNewlineTabAndSpace(const char c, uint32_t i) {
		if (c == '\n' || c == '\t' || c == ' ') {
			return true;
		}
		return false;
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
		uint32_t limit = UINT32_MAX, bool (*discardMatchConditional)(const char*, const char*, const char*) = NULL,
		bool (*ignoreCharacterConditional)(const char /*char*/, const uint32_t /*char index*/) = NULL)
	{
		const uint32_t targlen = strlen(targ);
		COMMENT_TYPE comment = NONE;
		for (uint32_t i = 0; i < limit; ++i) {
			if (cstr[i] == '\0') {
				return NULL;
			}

			if (ignoreCharacterConditional) {
				bool ignoreChar = ignoreCharacterConditional(cstr[i], i);
				if (ignoreChar)
				{
					continue;
				}
			}

			if (ignoreCommented) {
				handleComment(&comment, cstr + i);
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
	char* findNextMatchAdditive(const char* cstr, const char* targ, uint32_t limit = UINT32_MAX, const bool ignoreCommented = true,
		bool (*discardMatchConditional)(const char*, const char*, const char*) = NULL,
		bool (*ignoreCharacterConditional)(const char /*char*/, const uint32_t /*char index*/) = NULL)
	{
		char* nextMatch = findNextMatch(cstr, targ, ignoreCommented, limit, discardMatchConditional, ignoreCharacterConditional);
		if (nextMatch) {
			return nextMatch + strlen(targ);
		}
		return NULL;
	}

	int64_t getClosingFigureOffset(const char* cstr, const char openingFig, const char closingFig, const bool ignoreCommented = true) {
		uint32_t nestCount = 0u;
		uint32_t i = 0;
		COMMENT_TYPE comment = NONE;
		while (true)
		{
			if (cstr[i] == '\0') {
				return -1;
			}

			if (ignoreCommented) {
				handleComment(&comment, cstr + i);
			}

			if (comment == NONE) {
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
		int64_t tmp = getClosingFigureOffset(cstr, '(', ')', ignoreComments);
		if (tmp < 0) {
			return NULL;
		}
		return cstr + tmp;
	}

	// returns a pointer to the closing bracket. '}'
	// The cstr must be a pointer to the beginning bracket '{'
	// If it can't find a closing bracket, NULL will be returned
	const char* getClosingBracket(const char* cstr, const bool ignoreComments = true) {
		if (cstr[0] != '{') {
			throw std::exception("The cstr of getClosingParenthesis must be a pointer to '{'");
		}
		int64_t tmp = getClosingFigureOffset(cstr, '{', '}', ignoreComments);
		if (tmp < 0) {
			return NULL;
		}
		return cstr + tmp;
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
		else if (keyword == FIXED_BEGIN_KEYWORD) {
			return 1 + kywrdBegin + strlen(FIXED_BEGIN_KEYWORD);
		}
		else if (keyword == FIXED_END_KEYWORD) {
			return 1 + kywrdBegin + strlen(FIXED_END_KEYWORD);
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

		char* lastOpeningParenthesis = (char*)cstr;
		while (true)
		{
			if (cstr[i] == '\0') {
				return NULL;
			}

			handleComment(&comment, cstr + i);

			if (comment == NONE) {
				if (cstr[i] == '(') {
					lastOpeningParenthesis = (char*)cstr + i;
					const char* closeParenthesis = getClosingParenthesis(cstr + i);
					if (closeParenthesis == NULL) {
						return NULL; // failed to find closing parenthesis
					}
					// jump to the closing parenthesis
					i += uint64_t(closeParenthesis - lastOpeningParenthesis);
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
	void readArgs(const char* str, struct ARG_BLOCK* argBlock, const uint32_t charLimit, char seperatingCharacter = ',') {
		// reads arguments seperated by ,
		// note that double commas ',,' will be treated as ','. This aligns with C standards.

		if (str[0] == '(') {
			throw std::exception("The str argument in readArgs must NOT begin with '('");
		}

		argBlock->args = NULL;

		// arglist - a contigious block of memory divided by the seperator list
		char*& arglist = argBlock->args;
		uint16_t& argCount = argBlock->argCount;
		uint32_t arglistByteSize = 0u;

		COMMENT_TYPE comment = NONE;

		uint32_t argclen=0u;

		for (uint32_t i = 0; i < charLimit; i++) {

			if (str[i] == '\0') {
				dbg::printWarning("Stopped before char limit for readArgs");
				return;
			}

			handleComment(&comment, str + i);

			if (!comment) {

				// we must jump past any opening brackets or parentheses,
				// as these are assumed to be the constructors of default values
				// of the arguments of the pass keyword
				if (str[i] == '(') {
					const char* cpar = getClosingParenthesis(str + i);
					if (cpar) {
						const uint32_t jmplen = cpar - (str + i);
						i += jmplen;
						argclen += jmplen;
					}
				}
				else if (str[i] == '{') {
					const char* cbra = getClosingBracket(str + i);
					if (cbra) {
						const uint32_t jmplen = cbra - (str + i);
						i += jmplen;
						argclen += jmplen;
					}
				}



				// ignore any leading or trailing spaces, but mind that spacing in-between words are important and must be kept
				const char tmp = str[i];
				if (str[i] == seperatingCharacter || i == charLimit - 1) {

					if (argclen > 0) {

						// check for any trailing spaces,
						// wind back argclen until they 
						// are not considered as part of the arg
						uint16_t j = 0;
						while (isCharBlank(str[i - j])) {
							j++;
							argclen--;
						}
						uint32_t offset = i - j;
						if (str[i] == ',' && offset!=0) {
							//offset--; // -1 to not include the comma
						}

						// allocate argument and copy it
						arglist = (char*)realloc(arglist, arglistByteSize + argclen + 1);
						memcpy(arglist + arglistByteSize, str + offset - argclen, argclen);
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
				// note that blank chars, like spaces, are allowed inside an argument,
				// but any leading blank characters must be ignoreed
				if (!(isCharBlank(str[i]) && argclen==0)) {
					argclen++;
				}
			}
		}
	}

	VAL_RETURN_CODE RENDER_GRAPH::loadFromFile(const std::filesystem::path& filepath) 
	{
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
				dbg::printError("Failed to close render graph source file");
				retCode = VAL_FAILURE;
			}
		}
		else {
			dbg::printError("Failed to load render graph source file %s", srcFileName.c_str());
			retCode = VAL_FAILURE;
		}

		return retCode;
	}



	VAL_RETURN_CODE RENDER_GRAPH::compile(const uint8_t framesInFlight, const filepath& compileToDir) {

		if (strlen(srcFileContents)==0) {
			dbg::printError("Failed to compile render graph, the src file has not been loaded. Perhaps you forgot to call loadFromFile?");
			return VAL_FAILURE;
		}



		char* errorMsg;
		string processed_src;
		const VAL_RETURN_CODE preprocess_res = preprocess(&processed_src, &errorMsg, framesInFlight);
		if (preprocess_res == VAL_FAILURE) {
			printf("Failed to compile render pass: %s\n", errorMsg);
			cleanup();
			return VAL_FAILURE;
		}


		string srcFilepathNO_EXT = removeAllFileExtensions(srcFileName);
		string srcFilepathNO_EXT_NO_PARENT_DIRS = removeParentDirs(srcFilepathNO_EXT);

		string processedFileName;
		if (compileToDir.empty() == true) {
			processedFileName = srcFilepathNO_EXT_NO_PARENT_DIRS;
		}
		else {
			processedFileName = compileToDir.string() + "/" + srcFilepathNO_EXT_NO_PARENT_DIRS;
		}
		processedFileName.append("__processed").append(".hpp");



		{
			// the source file contents need to be placed in a file after preprocessing.
			FILE* processedSrcFptr = NULL;
			errno_t openErr = fopen_s(&processedSrcFptr, processedFileName.c_str(), "wb");
			if (openErr == EEXIST || processedSrcFptr == NULL) {
				if (processedSrcFptr) {
					fclose(processedSrcFptr);
				}
				dbg::printError("Failed to open processed header file for writing, this file already exists or is locked!% s\n", processedFileName.c_str());
				return VAL_FAILURE;
			}

			if (processed_src.length() > 0) {
				fwrite(processed_src.c_str(), processed_src.length(), 1, processedSrcFptr);
			}
			else {
				if (processedSrcFptr) {
					fclose(processedSrcFptr);
				}
				printf("Nothing to compile, the processed src length is 0!\n");
				return VAL_FAILURE;
			}
			fclose(processedSrcFptr);
		}

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

	VAL_RETURN_CODE RENDER_GRAPH::readPass(struct PASS_INFO* __passInfo__,
		char* passBegin, uint32_t* passStrLen, char** error)
	{
#ifndef NDEBUG
		if (!__passInfo__ || !passBegin) {
			*error = (char*)"Failed to read pass, missing arguments!";
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
				*error = (char*)"Name opening parenthesis '(' is missing";
				return VAL_FAILURE;
			}
			const char* nameClosingBracket = getClosingParenthesis(nameOpeningParen);
			if (nameClosingBracket == NULL) {
				*error = (char*)"Name closing parenthesis ')' is missing";
				return VAL_FAILURE;
			}
			uint8_t passNameLen = nameClosingBracket - nameOpeningParen - 1;
		
			// alloc pass name
			passInfo.passName = (char*)realloc(passInfo.passName, passNameLen + 1);
			if (passInfo.passName == NULL) {
				*error = (char*)"Out of memory for pass name!";
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
					*error = (char*)"Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.readBlock, argsEndParenthesis - argsBeginParenthesis);
				
				// set cur to closing ")"
				cur = argsEndParenthesis;
			}
		}

		//cur += nextLine(cur);

		// get writes
		{
			// search for "WRITE("
			KEYWORD writeKeyword = findNextMatchAdditive(cur, WRITE_KEYWORD);
			if (writeKeyword) {
				char* argsBeginParenthesis = (char*)writeKeyword;
				cur = argsBeginParenthesis + 1;
				// search for closing ')'
				char* argsEndParenthesis = (char*)getClosingParenthesis(argsBeginParenthesis);
				if (argsEndParenthesis == NULL) {
					*error = (char*)"Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.writeBlock, argsEndParenthesis - argsBeginParenthesis);

				// set cur to closing ")"
				cur = argsEndParenthesis;
			}
		}

		// get read writes
		{
			//extractPassData(&passInfo, READ_WRITE);
			// search for "WRITE("
			KEYWORD rwKeyword = findNextMatchAdditive(cur, READ_WRITE_KEYWORD);
			if (rwKeyword) {
				char* argsBeginParenthesis = (char*)rwKeyword;
				cur = argsBeginParenthesis + 1;
				// search for closing ')'
				char* argsEndParenthesis = (char*)getClosingParenthesis(argsBeginParenthesis);
				if (argsEndParenthesis == NULL) {
					*error = (char*)"Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.readWriteBlock, argsEndParenthesis - argsBeginParenthesis);

				// set cur to closing ")"
				cur = argsEndParenthesis;
			}
		}
		// get input
		{
			KEYWORD inputKeyword = findNextMatchAdditive(cur, INPUT_KEYWORD);
			if (inputKeyword) {
				char* argsBeginParenthesis = (char*)inputKeyword;
				cur = argsBeginParenthesis + 1;
				// search for closing ')'
				char* argsEndParenthesis = (char*)getClosingParenthesis(argsBeginParenthesis);
				if (argsEndParenthesis == NULL) {
					*error = (char*)"Closing ')' is missing";
					return VAL_FAILURE;
				}

				// read arguments
				readArgs(cur, &passInfo.inputBlock, argsEndParenthesis - argsBeginParenthesis);

				// set cur to closing ")"
				cur = argsEndParenthesis;
			}
		}

		KEYWORD passEnd = NULL;
		// check for end
		{
			passEnd = findNextMatch(cur, PASS_END_KEYWORD, true, UINT32_MAX, discardMatchIfNoLeadingSpaceOrNewline, NULL);
			if (passEnd == NULL) {
				*error = (char*)"PASS_BEGIN is missing END_PASS";
				return VAL_FAILURE;
			}

			// set pass string length
			*passStrLen = passEnd - passBegin;
		}

		const char* execBegin = findExecSrcBegin(passBegin);

		// the exec src opening bracket '{' should be directly after the last keyword.
		if (!execBegin) {
			*error = (char*)"Failed to find the beginning of the pass exec src";
			return VAL_FAILURE;
		}
		const char* execBeginBracket = execBegin - 1;

		///////////////////////////////////////////////////////////////////////
		// get fixed subroutines
		{
			char* cur = passBegin;
			for (uint32_t i = 0; i < UINT32_MAX; ++i) {
				if (cur >= passEnd) {
					break;
				}
				uint32_t distToEnd = passEnd - cur;
				const char* fixedBegin = findNextMatchAdditive(cur, FIXED_BEGIN_KEYWORD, distToEnd, true, discardMatchIfNoLeadingSpaceOrNewline);
				if (!fixedBegin) {
					break; // no more fixed passes
				}

				const char* fixedArgBeginParen = findNextMatch(fixedBegin, "(");
				if (NULL == fixedArgBeginParen) {
					*error = (char*)"Failed to parse fixed subroutine: missing '(' of arg-begin";
					return VAL_FAILURE;
				}
				const char* fixedArgEndParen = getClosingParenthesis(fixedArgBeginParen);
				
				if (NULL == fixedArgEndParen) {
					*error = (char*)"'(' is missing respective ')'";
					return VAL_FAILURE;
				}
				
				// read fixed arguments

				ARG_BLOCK fixedArgs{};
				readArgs(fixedArgBeginParen+1, &fixedArgs, fixedArgEndParen - fixedArgBeginParen - 2, ';');

				// read fixed source
				const char* fixedSrcBegin = fixedArgEndParen + 1;
				if (*fixedSrcBegin == '\0');

				char* fixedEnd = findNextMatch(cur, FIXED_END_KEYWORD, distToEnd);
				if (!fixedEnd)
				{
					*error = (char*)"FIXED_BEGIN is missing FIXED_END";
					return VAL_FAILURE;
				}
				fixedEnd -= 1; //-1 because fixedEnd points to the 'F' in FIXED_NED

				cur = (char*)fixedEnd;

				const uint64_t fixedSubroutineLength = fixedEnd - fixedSrcBegin;
				
				// add the block to passInfo
				passInfo.fixedBlockCount++;

				FIXED_BLOCK* tmpFixedBlocks =
					(FIXED_BLOCK*)realloc(passInfo.fixedBlocks, 
						sizeof(tmpFixedBlocks[0]) * passInfo.fixedBlockCount);

				FIXED_BLOCK* lastFixedBlock = NULL;
				if (tmpFixedBlocks) {
					passInfo.fixedBlocks = tmpFixedBlocks;
					lastFixedBlock = &(passInfo.fixedBlocks[passInfo.fixedBlockCount - 1]);

					lastFixedBlock->srcOffset = fixedSrcBegin - execBeginBracket;
					lastFixedBlock->srcLength = fixedSubroutineLength;
				}
				else {
					passInfo.fixedBlockCount--;
					*error = (char*)"Out of system memory, could not allocate FIXED_BLOCKS information!";
					return VAL_FAILURE;
				}

				// add fixed args to fixed block info
				if (fixedArgs.argCount != 2) {
					ARG_BLOCK_DESTROY(&fixedArgs);
					*error = (char*)"Invalid argument count for fixed subroutine: every fixed subroutine must have exactly 3 arguments,"
						"a VkRenderPass, val::subpass, and a framesInFlight value, in that respective order.";
					return VAL_FAILURE;
				}
				else {

					const char* framebuffer_A_name = GET_ARG_FROM_ARG_BLOCK(&fixedArgs, 0);
					strcpy_s(lastFixedBlock->renderPassArgName, sizeof(lastFixedBlock->renderPassArgName)-1, framebuffer_A_name);


					const char* subpass_A_name = GET_ARG_FROM_ARG_BLOCK(&fixedArgs, 1);
					strcpy_s(lastFixedBlock->subpassArgName, sizeof(lastFixedBlock->subpassArgName) - 1, subpass_A_name);

					//const char* framesInFlight_A_name = GET_ARG_FROM_ARG_BLOCK(&fixedArgs, 2);
					//strcpy_s(lastFixedBlock->framesInFlightArgName, sizeof(lastFixedBlock->framesInFlightArgName) - 1, framesInFlight_A_name);

					ARG_BLOCK_DESTROY(&fixedArgs);

				}
 			}
		}

		// get exec src
		{

			const char* execClosingBracket = getClosingBracket(execBeginBracket);

			if (execBegin == NULL || execBeginBracket == NULL || execClosingBracket == NULL) {
				*error = (char*)"Error parsing pass exec, possible missing '(' or ')'";
				// failed to find exec src
				return VAL_FAILURE;
			}


			if (passInfo.execSrc) {
				free(passInfo.execSrc);
			}
			passInfo.execSrcLen = uint32_t(execClosingBracket - execBeginBracket - 2);
			// passInfo must own a copy of exec begin, as per the standards.
			passInfo.execSrc = (char*)malloc(passInfo.execSrcLen + 1);
			if (passInfo.execSrc) {
				memcpy(passInfo.execSrc, execBeginBracket + 1, passInfo.execSrcLen);
				// null terminate
				passInfo.execSrc[passInfo.execSrcLen] = '\0';
			}
			else {
				*error = (char*)"Out of system memory, could not allocate memory for passInfo.execSrc";
				return VAL_FAILURE;
			}

			printf("-- %.*s --\n", passInfo.execSrcLen, passInfo.execSrc);

			//extractPassData(&passInfo, &EXEC_SRC);
		}


		return VAL_SUCCESS;
	}

	
	string getPassMainFuncName(const char* passName) {
		return string("pass_main" + string(passName));
	}
	string getPassMainFuncSig(const char* passName) {
		return string("void pass_main" + string(passName));
	}
	string getPassBakeFuncName(const char* passName) {
		return string("pass_bake" + string(passName));
	}
	string getPassBakeFuncSig(const char* passName) {
		return string("void pass_bake" + string(passName));
	}

	void getCommandBufferName(char* buff, size_t buffSize, const char* passName, uint32_t fixedSubroutineIndex) {
		snprintf(buff, buffSize - 1, "__%s_fixed_cmd_buffer_%d", passName, fixedSubroutineIndex);
		buff[buffSize-1] = '\0';
	}

	VAL_RETURN_CODE processFixedPass(const PASS_INFO* passInfo, string& processedSrc, const char* fixedBlockSrc, uint32_t blockSrcLen, uint32_t fixedBlockIndex, uint8_t framesInFlight) {

		FIXED_BLOCK& fixedBlock = passInfo->fixedBlocks[fixedBlockIndex];

		const string passMainName = getPassMainFuncName(passInfo->passName);
		// choose a name for the fixed cmd buffer.
		char cmdBuffName[128]; 
		getCommandBufferName(cmdBuffName, sizeof(cmdBuffName), passInfo->passName, fixedBlockIndex);


		char* passBeginPTR = findNextMatch(processedSrc.c_str(),
			getPassMainFuncSig(passInfo->passName).c_str(), true, UINT32_MAX, discardMatchIfNoLeadingSpaceOrNewline);

		if (passBeginPTR == NULL) return VAL_FAILURE;

		uint32_t passBeginOffset = passBeginPTR - processedSrc.data();

		// add command buffer just above pass_main
		processedSrc.insert(passBeginOffset, ";\n");
		processedSrc.insert(passBeginOffset, string(cmdBuffName) + string("[") + std::to_string(framesInFlight) + string("]"));
		processedSrc.insert(passBeginOffset, "VkCommandBuffer ");

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	
		// add buffer initialization
		processedSrc.append("\n{\n");
		processedSrc.append(
			string("\nVkCommandBufferAllocateInfo allocInfo;\n"
				"allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;\n"
				"allocInfo.pNext = VK_NULL_HANDLE;\n"
				"allocInfo.commandPool = V_PROC._commandPool;\n"
				"allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;\n"));
		processedSrc.append(
			string("allocInfo.commandBufferCount = ") + std::to_string(framesInFlight) + string(";\n")
		);
		processedSrc.append(
			string("if (vkAllocateCommandBuffers(V_PROC._device, &allocInfo,") + cmdBuffName + string(") != VK_SUCCESS) {\n"
					"throw std::runtime_error(\"Failed to allocate command buffers!\");\n"
			"}\n")
		);
		processedSrc.append("\n}\n");


		
		processedSrc.append("for (uint8_t " + getCurrentFrameIndexArgName() + " = 0;"
			+ getCurrentFrameIndexArgName() + " < " + std::to_string(framesInFlight) 
			+ ";++" + getCurrentFrameIndexArgName() + ") { \n");

		// begin command buffer recording
		processedSrc.append(
			"\n{\n"
			"/* BEGIN RECORDING */\n"
			"VkCommandBufferInheritanceInfo inheritanceInfo = {\n"
			".sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,\n"
		);


		size_t spacePos = string(fixedBlock.renderPassArgName).find(' ');
		std::string renderpass_a_name = string(fixedBlock.renderPassArgName).substr(spacePos + 1);
		spacePos = string(fixedBlock.subpassArgName).find(' ');
		std::string subpass_a_name = string(fixedBlock.subpassArgName).substr(spacePos + 1);

		processedSrc.append(string(".renderPass = ") + renderpass_a_name + string(",\n"));
		processedSrc.append(string(".subpass = ") + subpass_a_name + "};\n");

		processedSrc.append(
			"VkCommandBufferBeginInfo beginInfo{};\n"
			"beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;\n"
			"beginInfo.pInheritanceInfo = &inheritanceInfo;\n"
			"beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;  // Or 0 if outside render pass\n\n"
			"vkBeginCommandBuffer(" + string(cmdBuffName) + "[" + getCurrentFrameIndexArgName() + "]" + ", &beginInfo);\n"
			"\n}\n"
		);
		// find render graph functions and replace any instances of command buffers with the fixed command buffer
		const char* f_table[] = {
			("setPipeline(*,*,c)"), /*the extra parenthesis are to reduce the risk of joined string errors*/
			("setPipeline("),		/*(C will automatically join strings together if they're not seperated by a comma)*/
			("setViewport(*,c)"),
			("setViewport("),
			("setScissor(*,c)"),
			("setScissor("),
			("setIndexBuffer(*,c)"),
			("setIndexBuffer("),
			("setVertexBuffer(*,c)"),
			("setVertexBuffer("),
			("drawInstanced(*,*,*,c)"),
			("drawInstanced("),
			("drawInstanced(*,*,c)"),
			("drawInstanced("),
			("drawIndexed(*,c)"),
			("drawIndexed("),
			("draw(*,c)"),
			("draw(")
		};

	#ifndef NDEBUG
		if (ARR_COUNT(f_table) % 2 != 0) {
			assert("ERROR: F_TABLE MUST HAVE A LENGTH THAT IS A MULTIPLE OF 2");
		}
	#endif // !NDEBUG

		// scan the fixed subroutine statement by statement, as seperated by ';'
		char* cur = (char*)fixedBlockSrc;
		for (uint32_t i = 0; i < UINT32_MAX; ++i)
		{
			const char* statementBegin = cur;
			const char* statementEnd = findNextMatch(cur, ";");
			if (!statementEnd) {break;}

			const uint32_t statementLen = statementEnd - statementBegin;
			uint32_t expectedArgCount = 0u;
			uint32_t expectedCmdArgIdx = 0u;
			uint16_t f_table_f_idx = 0u;
			char* fmatch = NULL;
			// check if the current statment matches a function in the function table
			for (uint32_t j = 0; j < ARR_COUNT(f_table) / 2; ++j) {
				const char* tfunc = f_table[j * 2 + 1];
				fmatch = findNextMatchAdditive(cur, tfunc, statementLen+1);
				if (fmatch) {

					// note that the data calculated here could be cached in a hash map
					f_table_f_idx = j;
					// calculate expected argument count by checking function table
					const char* o_par = findNextMatch(f_table[j * 2], "(");
					const char* c_par = getClosingParenthesis(o_par);
					ARG_BLOCK f_args{};
					readArgs(o_par+1, &f_args, c_par - o_par);
					expectedArgCount = f_args.argCount;

					for (uint16_t k = 0; k < f_args.argCount; ++k) {
						const char* arg = GET_ARG_FROM_ARG_BLOCK(&f_args, k);
						if (streql(arg, "c") == true) {
							expectedCmdArgIdx = k;
						}
					}

					ARG_BLOCK_DESTROY(&f_args);

					// we found the matching function, stop further checks
					break;
				}
			}

			if (!fmatch) {
				// insert source statement
				processedSrc.append(statementBegin, statementEnd + 1);

				i += statementEnd - statementBegin + 1;
				// jump to the 1 past the end of the statement
				cur += statementEnd - statementBegin + 1;
				continue; // the statement is not a render pass function, skip it
			}

			const char* cpar = getClosingParenthesis(fmatch - 1);

			// now if the statement matches
			ARG_BLOCK f_args {};
			readArgs(fmatch, &f_args, cpar - fmatch);
			
			processedSrc.append((char*)statementBegin, (char*)fmatch);

			for (uint16_t j = 0; j < f_args.argCount; ++j) {
				char* cmdArg = GET_ARG_FROM_ARG_BLOCK(&f_args, j);
				if (j == expectedCmdArgIdx) {
				// remove existing cmd buffer and replace it with fixed command buffer
					processedSrc.append(string(cmdBuffName) + "[" + getCurrentFrameIndexArgName() + "]");
				}
				else {
					processedSrc.append(cmdArg);
				}
				if (j != f_args.argCount - 1) {
					processedSrc.append(",");
				}
			}
 
			processedSrc.append((char*)cpar, (char*)statementEnd + 1);

			i += statementEnd - statementBegin + 1;
			// jump to the 1 past the end of the statement
			cur += statementEnd - statementBegin + 1;

			ARG_BLOCK_DESTROY(&f_args);
		}

		// end command buffer recording
		processedSrc.append(
			"\n{\n"
			"/* END RECORDING */\n"
			"vkEndCommandBuffer(" + string(cmdBuffName) + "[" + getCurrentFrameIndexArgName() + "]" + ");"
			"\n}\n"
		);

		// close for loop
		processedSrc.append("}\n");

		// if there's anything left other, append it
		if (!(cur >= (char*)fixedBlockSrc + blockSrcLen)) {
			processedSrc.append(cur, (char*)fixedBlockSrc + blockSrcLen);
		}


		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE RENDER_GRAPH::preprocess(string* processed_src_out, char** errorMsg, const uint8_t framesInFlight)
	{
		char* src = srcFileContents;

		if (!src || !processed_src_out || !(*errorMsg)) {
			dbg::printError("Failed to preprocess file, invalid arguments!\n");
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
		* #include <VAL/lib/renderGraph/pass.hpp> <- note that everything before the first pass must be kept in the processed src file
		* 
		* PASS_BEGIN(DRAW_RECT)
		* READ(buffer& vertexBuffer, buffer& indexBuffer)
		* WRITE(NULL)
		* READ_WRITE(NULL)
		* FIXED
		* INPUT(graphicsPipelineHdl& pipeline, window& wind)
		* {
		* 	// execute pass
		* }
		* PASS_END
		* 
		* <- anything below the last pass will be discarded, and if anything besides a comment is there
		*	 a warning should be printed
		*/

		string& processedSrc = *processed_src_out;


		PASS_INFO* passInfos = NULL;
		uint16_t passInfoCount = 0u;
		
		string srcBeforeFirstPass;

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
					dbg::printError("Failed to preprocess render pass: %s", readerr);
					goto bail;

					return VAL_FAILURE;
				}
				
				//PRINT_ARG_BLOCK(&(curPassInfo->readBlock));
			}

			if (passInfoCount == 0) {
				srcBeforeFirstPass.push_back(src[cur]);
			}

			cur++;
		}

		processedSrc.insert(0, srcBeforeFirstPass);
		processedSrc.insert(0, "#include <VAL/lib/system/VAL_PROC.hpp>\n");

		/* 
		* Deduplicate arguments, and append the them in this order:
		* READ (ARGS) 
		* WRITE (ARGS)
		* READ_WRITE (ARGS)
		* INPUT (ARGS)
		*/

		//const ARG_BLOCK deduplicatedArgs = passInfos[0].in



		// add a define, which will be used to call that function
		//processedSrc.append("#define " "PASS_MAIN() \n");


		// all passes have all been read sucessfully, write to the src
		// note that the compiled src file should use (void pass_main()) as the entry point

		for (uint16_t i = 0u; i < passInfoCount; ++i) {
			//////////////////////////////////////////////////////////////////////
			// PASS MAIN //
			const auto& passInfo = passInfos[i];
			{
				// main pass function begin
				processedSrc.append(getPassMainFuncSig(passInfo.passName) + "(");


				//////////////////////////////////////////////////////////////////////
				// add V_PROC
				processedSrc.append("val::VAL_PROC& V_PROC,");

				//////////////////////////////////////////////////////////////////////
				// add read args
				processedSrc.append(argBlockToString(passInfo.readBlock));
				// add write args
				processedSrc.append(argBlockToString(passInfo.writeBlock));
				// add read write args
				processedSrc.append(argBlockToString(passInfo.readWriteBlock));
				// add input args
				processedSrc.append(argBlockToString(passInfo.inputBlock));
				//////////////////////////////////////////////////////////////////////

				removeTrailingComma(processedSrc);

				processedSrc.append(") {\n");

				// this allows to preprocessor to seperate exec src from fixed subroutines. 
				char* last_exec = passInfo.execSrc;
				// handle fixed subroutines
				for (auto j = 0; j < passInfo.fixedBlockCount; ++j) {

					FIXED_BLOCK& fixedBlock = passInfo.fixedBlocks[j];

					char* fixedBlockSrc = passInfo.execSrc + fixedBlock.srcOffset;
					string execGap(last_exec, fixedBlockSrc);
					processedSrc.append(execGap);

					// https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdExecuteCommands.html
					char cmdBuffName[128];
					getCommandBufferName(cmdBuffName, sizeof(cmdBuffName), passInfo.passName, j);
					processedSrc.append(string("\n\t\tvkCmdExecuteCommands(") + string("cmd,1, &(") +
						string(cmdBuffName) + "[V_PROC.getCurrentFrame()]" + string("));\n"));
					//processFixedPass(&passInfo, processedSrc, fixedBlockSrc, fixedBlock.srcLength, i);

					last_exec = passInfo.execSrc + fixedBlock.srcOffset + fixedBlock.srcLength;
				}
				string execGap(last_exec, passInfo.execSrc + passInfo.execSrcLen);
				processedSrc.append(execGap);
				
				processedSrc.append("\n}\n");
			}



			//////////////////////////////////////////////////////////////////////
			// PASS BAKING //
			{
				if (passInfo.fixedBlockCount > 0) {
					// pass bake function begin
					processedSrc.append(getPassBakeFuncSig(passInfo.passName) + "(");

					//////////////////////////////////////////////////////////////////////
					// add V_PROC
					processedSrc.append("val::VAL_PROC& V_PROC,");

					// add read args
					processedSrc.append(argBlockToString(passInfo.readBlock));
					// add write args
					processedSrc.append(argBlockToString(passInfo.writeBlock));
					// add read write args
					processedSrc.append(argBlockToString(passInfo.readWriteBlock));
					// add input args
					processedSrc.append(argBlockToString(passInfo.inputBlock));

					processedSrc.append(passInfo.fixedBlocks[0].renderPassArgName);
					processedSrc.append(",");
					processedSrc.append(passInfo.fixedBlocks[0].subpassArgName);
					//processedSrc.append(",");
					//processedSrc.append(passInfo.fixedBlocks[0].framesInFlightArgName);

					//////////////////////////////////////////////////////////////////////

					removeTrailingComma(processedSrc);

					processedSrc.append(") {\n");


					const auto filterExecGap = [](string& execGap) {
						// remove any FIXED_BEGIN (args...) or FIXED_END Keywords 
						// and replace them with '{' and '}' respectively

						// remove FIXED_BEGIN
						const char* beginKeywordStart = findNextMatch(execGap.c_str(), FIXED_BEGIN_KEYWORD, true, UINT32_MAX, discardMatchIfNoLeadingSpaceOrNewline, NULL);
						if (beginKeywordStart != NULL) {
							const char* pBegin = findNextMatch(beginKeywordStart + strlen(FIXED_BEGIN_KEYWORD), "(");
							if (!pBegin) {
								return false;
							}
							const char* pEnd = getClosingParenthesis(pBegin);
							if (!pEnd) {
								return false;
							}
							size_t start = beginKeywordStart - execGap.c_str();
							size_t end = pEnd - execGap.c_str();

							execGap.replace(start, end - start + 1, "{");
						}

						// remove FIXED_END
						const char* endKeywordStart = findNextMatch(execGap.c_str(), FIXED_END_KEYWORD, true, UINT32_MAX, discardMatchIfNoLeadingSpaceOrNewline, NULL);
						if (endKeywordStart) {
							size_t start = endKeywordStart - execGap.c_str();
							size_t end = (endKeywordStart + strlen(FIXED_END_KEYWORD)) - execGap.c_str();

							execGap.replace(start, end - start + 1, "}");
						}

						return true;
					};

					// this allows to preprocessor to seperate exec src from fixed subroutines. 
					char* last_exec = passInfo.execSrc;
					// handle fixed subroutines
					for (auto j = 0; j < passInfo.fixedBlockCount; ++j) {

						FIXED_BLOCK& fixedBlock = passInfo.fixedBlocks[j];

						char* fixedBlockSrc = passInfo.execSrc + fixedBlock.srcOffset;
						string execGap(last_exec, fixedBlockSrc);
						
						bool res = filterExecGap(execGap);
						if (res == false) {
							goto bail;
						}



						processedSrc.append(execGap);

						processFixedPass(&passInfo, processedSrc, fixedBlockSrc, fixedBlock.srcLength, j, framesInFlight);

						last_exec = passInfo.execSrc + fixedBlock.srcOffset + fixedBlock.srcLength;
					}
					string execGap(last_exec, passInfo.execSrc + passInfo.execSrcLen);
					bool res = filterExecGap(execGap);
					if (res == false) {
						goto bail;
					}
					processedSrc.append(execGap);

					processedSrc.append("\n}");
				}
			}
		}


		
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