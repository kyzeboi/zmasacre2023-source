/*

     ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#pragma once

#define _WIN32_WINNT                                0x0600
#define WIN32_LEAN_AND_MEAN                         1
#define _CRT_SECURE_NO_WARNINGS                     1
#define IMGUI_DEFINE_MATH_OPERATORS                 1
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS   1


// API/SDK includes
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <math.h>
#include <float.h>
#include <shellapi.h>
#include <d3dx9.h>
#include <WinSock2.h>
#include <Gdiplus.h>
#include <assert.h>
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <random>
#include <intrin.h>
#include <winres.h>
#include <TlHelp32.h>
#include <future>
#include <WS2tcpip.h>
#include <KnownFolders.h>
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include <tchar.h>
#include <ctime>
#include <chrono>

// GAME_SA Library
#include "CVector.h"
#include "CVector2D.h"
#include "CMatrix.h"
#include "CMatrix_Pad.h"
#include "SharedUtil.h"
#include <game/CGame.h>
#include <game/CWanted.h>
#include "SString.h"

// private SA classes
#include "CEntitySA.h"
#include "Common.h"
#include "CGameSA.h"
#include "CWorldSA.h"
#include "CPoolsSA.h"
#include "CClockSA.h"
#include "CFontSA.h"
#include "CRadarSA.h"
#include "CMenuManagerSA.h"
#include "CCameraSA.h"
#include "CCheckpointsSA.h"
#include "CRenderWareSA.h"
#include "CCoronasSA.h"
#include "CPickupsSA.h"
#include "CPathFindSA.h"
#include "CWeaponInfoSA.h"
#include "CExplosionManagerSA.h"
#include "CFireManagerSA.h"
#include "CHandlingManagerSA.h"
#include "CHudSA.h"
#include "C3DMarkersSA.h"
#include "CStatsSA.h"
#include "CTheCarGeneratorsSA.h"
#include "CPadSA.h"
#include "CAERadioTrackManagerSA.h"
#include "CWeatherSA.h"
#include "CTextSA.h"
#include "CPedSA.h"
#include "CPedSoundSA.h"
#include "CAudioSA.h"
#include "CPlayerInfoSA.h"
#include "CPopulationSA.h"
#include "CSettingsSA.h"
#include "CCarEnterExitSA.h"
#include "COffsets.h"
#include "CControllerConfigManagerSA.h"
#include "CProjectileInfoSA.h"
#include "CEventListSA.h"
#include "CGaragesSA.h"
#include "CTasksSA.h"
#include "CEventDamageSA.h"
#include "CEventGunShotSA.h"
#include "CEventGroupSA.h"
#include "CAnimManagerSA.h"
#include "CStreamingSA.h"
#include "CVisibilityPluginsSA.h"
#include "CKeyGenSA.h"
#include "CRopesSA.h"
#include "CFxSA.h"
#include "HookSystem.h"
#include "CModelInfoSA.h"
#include "CPedModelInfoSA.h"
#include "CColPointSA.h"
#include "CCivilianPedSA.h"
#include "CAnimBlendAssociationSA.h"
#include "CAnimBlendAssocGroupSA.h"
#include "CAnimBlendHierarchySA.h"
#include "CAnimBlendSequenceSA.h"
#include "CAnimBlendStaticAssociationSA.h"
#include "CAnimBlockSA.h"
#include "CAutomobileSA.h"
#include "CBikeSA.h"
#include "CBoatSA.h"
#include "CBmxSA.h"
#include "CQuadBikeSA.h"
#include "CMonsterTruckSA.h"
#include "CPlaneSA.h"
#include "CTrailerSA.h"
#include "CPlayerPedSA.h"
#include "CCivilianPedSA.h"
#include "CObjectSA.h"
#include "CBuildingSA.h"
#include "CRestartSA.h"
#include "CWaterManagerSA.h"
#include "CPedDamageResponseSA.h"
#include "CPedDamageResponseCalculatorSA.h"

// Discord
#include "discord_rpc.h"
#include "discord.h"

// ImGui Stuff
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "detours.h"

// Logger
#include "Logger.hpp"

// RakNet Stuff
#include "BitStream.h"
#include "Say so.h"

// Main Struct & Utils
#include "d3drender.h"
#include "Struct.h"

// Render Utils && Samp
#include "samp.h"
#include "Utils.h"

//Render Ýmgui
#include "RenderGUI.h"

// Render Keyboard 
#include "keyhook.h"

// Math Utils
#include "math_stuff.h"

// normal includes
#include "cDetour.h"