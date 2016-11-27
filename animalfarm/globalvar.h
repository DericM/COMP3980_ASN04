#pragma once

namespace GlobalVar
{
	/*Thread handles*/
	extern HANDLE hReadThread;
	extern HANDLE hWriteThread;
	extern HANDLE hIdleWaitThread;
	extern HANDLE g_hIdleSendENQThread;

	extern HANDLE g_hEnqEvent;

	extern BOOL g_bWaitENQ;
}