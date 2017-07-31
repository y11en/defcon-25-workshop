#pragma once

#include <Windows.h>
#include <Psapi.h>

static char * process_name(HANDLE hProc)
{
	static char szProcessName[MAX_PATH];
	HMODULE hMod;
	DWORD cbNeeded = 0;

	szProcessName[0] = '\0';

	if (EnumProcessModules(hProc, &hMod, sizeof(hMod), &cbNeeded))
		GetModuleBaseNameA(hProc, hMod, szProcessName, sizeof(szProcessName));

	return szProcessName;
}

static HANDLE find_process(const char *szProcessToFind)
{
	DWORD aProcesses[1024];
	DWORD cbNeeded = 0;
	DWORD cProcesses = 0;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return INVALID_HANDLE_VALUE;

	cProcesses = cbNeeded / sizeof(DWORD);

	for (DWORD i = 0; i < cProcesses; ++i)
	{
		if (aProcesses[i] != 0)
		{

			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,
				FALSE, aProcesses[i]);

			if (NULL == hProc)
				continue;

			if (_stricmp(szProcessToFind, process_name(hProc)) == 0)
				return hProc;

			CloseHandle(hProc);
		}
	}

	return INVALID_HANDLE_VALUE;
}