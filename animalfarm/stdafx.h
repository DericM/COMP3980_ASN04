// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//GLOBAL variables - in stdafx.h for now

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <sstream>

#ifdef _DEBUG
#define LOGMESSAGE( s )						\
{											\
   std::wostringstream os_;					\
   os_ << s;								\
   OutputDebugString( os_.str().c_str() );	\
}
#else
#define LOGMESSAGE( str )
#endif





// TODO: reference additional headers your program requires here
