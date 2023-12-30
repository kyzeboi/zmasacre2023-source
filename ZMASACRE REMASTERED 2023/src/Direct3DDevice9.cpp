/*

	 ** Bu proje kyze. tarafından geliştirilmektedir. **

*/

#include "main.h"
#include "Cheats.h"

CD3DHook*								pD3DHook;
D3DPRESENT_PARAMETERS					pPresentParam;

void Discord_İnit()
{
	if (Discord_RPC)
		Discord::update(NULL, "ZMasacre #2023", "Dene bir istersen...", "logo_2023", ".gg/wvFEuANKFY", "", "");
	else
		Discord_ClearPresence();
}

HRESULT __stdcall CD3DHook::hkPresent(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	if (!pDevice)
		return pD3DHook->oPresent(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);


	if (!pD3DHook->bInit)
	{
		pD3DHook->pD3DDevice = pDevice;

		// Render GUI
		İmMenu = new CMenu(pDevice);

		// Render Font's 
		render->Initialize(pDevice);

		// Load Fonts
		pD3DFont->Initialize(pDevice);
		pD3DFontFixedSmall->Initialize(pDevice);


		pD3DHook->bInit = true;
	}


	// Render SAMP
	renderSAMP();

	// Stuff
	Main_Hook();

	// İmGui
	RenderGUI();

	// Discord RPC
	Discord_İnit();

	// install aiming
	aimedplayer = GetAimingPlayer();

	// Cheats
	renderPlayerT4gs();
	RenderSpecialFuncs();

	return pD3DHook->oPresent(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall CD3DHook::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	if (!pDevice)
		return pD3DHook->oReset(pDevice, pPresentParams);


	pPresentParam = *pPresentParams;

	if (pD3DHook->bInit)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();

		// Font
		pD3DFont->Invalidate();
		pD3DFontFixedSmall->Invalidate();

		// Render Main
		render->Invalidate();

		pD3DHook->bInit = false;
	}



	return pD3DHook->oReset(pDevice, pPresentParams);

}