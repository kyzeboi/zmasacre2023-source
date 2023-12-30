/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#pragma once

class CMenu
{
public:
	CMenu(IDirect3DDevice9* pDevice)
	{
		show = false;
		ImGui::CreateContext();
		ImGui::GetIO().IniFilename = NULL;
		ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(GetActiveWindow());
		ImGui_ImplDX9_Init(pDevice);
		AddImGuiSettings();
	};

	~CMenu()
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	};


	void	AddImGuiSettings();
	void	RenderMenu();
	void	hkEndScene();

	bool show;
};

extern CMenu* ÝmMenu;


