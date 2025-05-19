#include "string.h"
#ifdef VAL_RENDERGRAPH_EXPORTS
#define VAL_RENDERGRAPH_API __declspec(dllexport)
#else
#define VAL_RENDERGRAPH_API __declspec(dllimport)
#endif
extern "C" VAL_RENDERGRAPH_API void pass_main();
