/*

	 ** Bu proje kyze. tarafından geliştirilmektedir. **

*/

//KRONDOR
//KYZE

#include "main.h"


HMODULE					  g_hDllModule = NULL;
HINSTANCE				  g_hOrigDll   = NULL;

void RenderConfig() {

	logger = std::make_unique<nLogger>();
}

int init()
{


	if (GetModuleFileName(g_hDllModule, g_szWorkingDirectory, sizeof(g_szWorkingDirectory) - 32) != 0)
	{
		if (strrchr(g_szWorkingDirectory, '\\') != NULL)
			*strrchr(g_szWorkingDirectory, '\\') = 0;
		else
			strcpy(g_szWorkingDirectory, ".");
	}
	else
	{
		strcpy(g_szWorkingDirectory, ".");
	}

	getSamp();
	RenderConfig();

	return 1;
}


void mainloop()
{
	if (init()) { pD3DHook = new CD3DHook(); }
}




BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
		
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);
			g_hDllModule = hModule;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainloop, NULL, NULL, NULL);
			_beginthread(&process, 0, nullptr);
			break;
		}
	}
	return true;
}
