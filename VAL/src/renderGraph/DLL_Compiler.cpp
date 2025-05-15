#include <VAL/lib/renderGraph/DLL_Compiler.hpp>

namespace val {

	COMPILE_RETURN_CODE compileToDLL(filepath srcPath, string DLLname, SUPPORTED_COMPILER compiler, const string EXPORT_FLAG, const COMPILE_ARGS& extraArgs) {
		COMPILE_RETURN_CODE retCode = COMPILE_SUCCESS;
		// first check if the file exists
		FILE* fptr = NULL;
		std::string srcPathStr = srcPath.string();
		fopen_s(&fptr, srcPathStr.c_str(), "rb");
		if (fptr) { fclose(fptr); }
		if (!fptr) { return COMPILE_FAILURE_SRC_FILE_INVALID; }
		fptr = NULL;


		const string cppStd = CPP_StandardToString(extraArgs.cppStandard);
		if (cppStd == "invalid") { return COMPILE_FAILURE_INVALID_CPP_VER; }

		if (MSVC == compiler) {
			// compiling DLLs with MSVC looks something like this
			// cl.exe /D_USRDLL /D_WINDLL <files-to-compile> <files-to-link> /link /DLL /OUT:<desired-dll-name>.dll

			// prepare command line arguments
			string cmpleDLL_CMD; /*compile object command*/
			cmpleDLL_CMD.reserve(64);
			cmpleDLL_CMD.append("cl /D_USRDLL /D_WINDLL " + srcPath.string() + " ");
			cmpleDLL_CMD.append(" /D " + EXPORT_FLAG + " ");
			cmpleDLL_CMD.append(" /std:" + cppStd + " ");
			// add linker files

			cmpleDLL_CMD.append(" /link /DLL /OUT:" + DLLname + ".dll ");

			// this starts up the Visual Studio Command Line, then the runs the cmpleDLL command 
			string vcvarsPath = R"("C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat")";

			string fullCommand = vcvarsPath + std::string(" && ") + cmpleDLL_CMD;
			if (system(fullCommand.c_str()) == -1) {
				retCode = COMPILE_FAILURE_CMD_FAIL;
				goto bail;
			}
		}

		else if (GPLUSPLUS == compiler)
		{
			string GPP = "g++";

			{	// first, compile to object file
				// prepare command line arguments
				string cmpleObjCMD; /*compile object command*/
				cmpleObjCMD.reserve(64);
				cmpleObjCMD.append(GPP);

				// set C++ standard
				cmpleObjCMD.append(" -std=" + cppStd);

				cmpleObjCMD.append(" -c -D " + EXPORT_FLAG + " " + srcPath.string() + " ");

				cmpleObjCMD.append(" -o " + DLLname + ".o ");
				if (system(cmpleObjCMD.c_str()) == -1) {
					retCode = COMPILE_FAILURE_CMD_FAIL;
					goto bail;
				}
			}
			{ // second, compile to DLL
				// prepare command line arguments
				string cmpleDLL_CMD; /*compile DLL command*/
				cmpleDLL_CMD.reserve(64);
				cmpleDLL_CMD.append(GPP);

				// set C++ standard
				cmpleDLL_CMD.append(" -std=" + cppStd);

				cmpleDLL_CMD.append(" -shared ");
				cmpleDLL_CMD.append(" -o " + DLLname + ".dll ");

				// in g++, -WL, option is used to pass arguments to the linker
				// ik that this seems trivial, but this will fail if there's spaces between -Wl,option,file.a

				cmpleDLL_CMD.append(" -Wl,--out-implib, " + DLLname + ".a ");
				cmpleDLL_CMD.append(DLLname + ".o ");

				if (system(cmpleDLL_CMD.c_str()) == -1) {
					retCode = COMPILE_FAILURE_CMD_FAIL;
					goto bail;
				}
			}
		}

		else if (CLANG == compiler)
		{
			retCode = COMPILE_FAILURE_UNSUPPORTED_COMPILER;
		}
		else {
			retCode = COMPILE_FAILURE_UNSUPPORTED_COMPILER;
		}


	bail:

		return retCode;
	}
}