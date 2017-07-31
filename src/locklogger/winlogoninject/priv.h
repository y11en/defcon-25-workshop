#pragma once

#include <Windows.h>

static BOOL set_token_privilege(
	HANDLE hToken,          // token handle
	LPCTSTR Privilege,      // Privilege to enable/disable
	BOOL bEnablePrivilege   // TRUE to enable.  FALSE to disable
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return FALSE;

	// 
	// first pass.  get current privilege setting
	// 
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious
	);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	// 
	// second pass.  set privilege based on previous setting
	// 
	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL
	);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	return TRUE;
}

#include <stdio.h>

static BOOL get_debug_priv()
{
	HANDLE hToken;

	printf("debug_prv()\n");

	OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken);

	if (GetLastError() == ERROR_NO_TOKEN)
	{
		if (!ImpersonateSelf(SecurityImpersonation))
			return FALSE;

		if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
			return FALSE;
	}


	printf("Got to token adjust\n");

	BOOL ret = set_token_privilege(hToken, SE_DEBUG_NAME, TRUE);
	
	CloseHandle(hToken);
	
	return ret;
}