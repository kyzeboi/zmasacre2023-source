/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#include "main.h"
#include "hookers.h"

/*                                                                                */

void sampMainCheat(void)
{
	// g_Vehicles & g_Players pointers need to be refreshed or nulled
	if (isBadPtr_writeAny(g_SAMP->pPools, sizeof(stSAMPPools)))
	{
		g_Vehicles = NULL;
		g_Players = NULL;
	}
	else if (g_Vehicles != g_SAMP->pPools->pVehicle || g_Players != g_SAMP->pPools->pPlayer)
	{
		if (isBadPtr_writeAny(g_SAMP->pPools->pVehicle, sizeof(stVehiclePool)))
			g_Vehicles = NULL;
		else
			g_Vehicles = g_SAMP->pPools->pVehicle;
		if (isBadPtr_writeAny(g_SAMP->pPools->pPlayer, sizeof(stPlayerPool)))
			g_Players = NULL;
		else
			g_Players = g_SAMP->pPools->pPlayer;
	}

	// Render Spectate
	spectateHandle();

	// update GTA to SAMP translation structures
	update_translateGTASAMP_vehiclePool();
	update_translateGTASAMP_pedPool();

	// Install HOOKS
	static int	iSAMPHooksInstalled;
	if (!iSAMPHooksInstalled)
	{
		installSAMPHooks();
		iSAMPHooksInstalled = 1;
	}


}


int			m_InitStages = 0;
void Main_Hook()
{
	// install keyhook
	keyhook_maybe_install(GetActiveWindow());

	player_state = CHEAT_STATE_NONE;

	/* setup & refresh actor pool */
	pool_actor = *(struct pool**)ACTOR_POOL_POINTER;
	if (pool_actor == nullptr || pool_actor->start == nullptr || pool_actor->size <= 0)
		return;

	/* setup & refresh vehicle pool */
	pool_vehicle = *(struct pool**)VEHICLE_POOL_POINTER;
	if (pool_vehicle == nullptr || pool_vehicle->start == nullptr || pool_vehicle->size <= 0)
		return;

	//////////////////////////////////////////
	struct actor_info* actor_info;
	struct vehicle_info* vehicle_info;
	actor_info = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
	vehicle_info = vehicle_info_get(VEHICLE_SELF, 0);

	/* no vehicle, and no actor. exit. */
	if (vehicle_info != nullptr && actor_info != nullptr)
	{
		if (vehicle_info == nullptr)
		{
			player_state = CHEAT_STATE_ACTOR;
		}
		else
		{
			player_state = CHEAT_STATE_VEHICLE;
		}

		// post GTA initialization initizalization routines
		if (m_InitStages == 0)
		{
			// setup CGame & friends
			pGameInterface = new CGameSA();
			pPools = pGameInterface->GetPools();

			// we have to add ourself to the pool first so that we are always the 1st ref
			// NEW --> use the global external: pPedSelf
			pPedSelf = pGameInterface->GetPools()->AddPed((DWORD*)actor_info);
			pPedSelfSA = pPedSelf->GetPedInterface();

			// install all startup hooks
			cheat_hookers_installhooks();

			// increment stage
			m_InitStages++;
		}
	}

	if (player_state != CHEAT_STATE_NONE)
	{
		// keep this updated, cos something is switching it now
		pPedSelf = pPools->GetPedFromRef(1);
		pPedSelfSA = pPedSelf->GetPedInterface();
	}


	if (g_SAMP && g_renderSAMP_initSAMPstructs == 1)
		sampMainCheat();


	if (gta_menu_active())	keyhook_clear_states(); else keyhook_run();
}

void renderSAMP()
{
	// get samp structures
	if (!g_renderSAMP_initSAMPstructs)
	{
		g_SAMP = stGetSampInfo();
		if (isBadPtr_writeAny(g_SAMP, sizeof(stSAMP)))
			return;

		if (isBadPtr_writeAny(g_SAMP->pPools, sizeof(stSAMPPools)))
			return;

		g_Players = g_SAMP->pPools->pPlayer;
		if (isBadPtr_writeAny(g_Players, sizeof(stPlayerPool)))
			return;

		g_Vehicles = g_SAMP->pPools->pVehicle;
		if (isBadPtr_writeAny(g_Vehicles, sizeof(stVehiclePool)))
			return;

		g_Chat = stGetSampChatInfo();
		if (isBadPtr_writeAny(g_Chat, sizeof(stChatInfo)))
			return;

		g_DialogInfo = stGetDialogInfo();
		if (isBadPtr_writeAny(g_DialogInfo, sizeof(stDialogInfo)))
			return;

		g_Input = stGetInputInfo();
		if (isBadPtr_writeAny(g_Input, sizeof(stInputInfo)))
			return;

		g_DeathList = stGetKillInfo();
		if (isBadPtr_writeAny(g_DeathList, sizeof(stKillInfo)))
			return;

		g_Scoreboard = stGetScoreboardInfo();
		if (isBadPtr_writeAny(g_Scoreboard, sizeof(stScoreboardInfo)))
			return;

		if (g_SAMP->pRakClientInterface == NULL)
			return;

		g_RakClient = new MyRakClient(g_SAMP->pRakClientInterface);
		g_SAMP->pRakClientInterface = new HookedRakClientInterface();

		g_renderSAMP_initSAMPstructs = 1;
	}




}

int sampPatchDisableNameTags(int iEnabled)
{
	static struct patch_set sampPatchEnableNameTags_patch =
	{
		"Remove player status",
		0,
		0,
		{
			{ 1, (void*)((uint8_t*)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_NAMETAGS), NULL, (uint8_t*)"\xC3", NULL },
			{ 1, (void*)((uint8_t*)g_dwSAMP_Addr + SAMP_PATCH_DISABLE_NAMETAGS_HP), NULL, (uint8_t*)"\xC3", NULL }
		}
	};
	if (iEnabled && !sampPatchEnableNameTags_patch.installed)
		return patcher_install(&sampPatchEnableNameTags_patch);
	else if (!iEnabled && sampPatchEnableNameTags_patch.installed)
		return patcher_remove(&sampPatchEnableNameTags_patch);
	return NULL;
}


const struct vehicle_entry* gta_vehicle_get_by_id(int id)
{
	id -= VEHICLE_LIST_ID_START;

	if (id < 0 || id >= VEHICLE_LIST_SIZE)
		return NULL;

	return &vehicle_list[id];
}

void RenderGUI()
{
	ÝmMenu->hkEndScene();

	if (ÝmMenu->show)
	{
		toggleSAMPCursor(1);
	}
	else
	{
		
		if (g_SAMP == NULL) return;
		if (g_Scoreboard->iIsEnabled) return;
		if (g_DialogInfo->iIsActive) return;
		if (!g_Players && !g_Vehicles)  return;

		if (girisyapildi)
			toggleSAMPCursor(0);
		
	}

}

/*                                                                                */


void GetAimingCenter(float& x, float& y)
{
	if (g_Players->pLocalPlayer->byteCurrentWeapon != 34)
	{
		x = (float)pPresentParam.BackBufferWidth * 0.5299999714f;
		y = (float)pPresentParam.BackBufferHeight * 0.4f;
	}
	else
	{

		x = (float)pPresentParam.BackBufferWidth / 2.f;
		y = (float)pPresentParam.BackBufferHeight / 2.f;
	}
}

bool AimingFiltersISSafe(int PlayerID)
{
	// NORMAL
	if (g_Players->iIsListed[PlayerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[PlayerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;
	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData->bytePlayerState == PLAYER_STATE_SPAWNED)
		return false;

	// SPECIAL
	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData->iAFKState == 2)
		return false;

	// FRIEND
	if (g_NewPatch.FriendID[PlayerID] && g_NewPatch.FriendDogrulama[PlayerID] && g_NewPatch.FriendName[PlayerID] == getPlayerName(PlayerID))
		return false;

	// ENEMY
	if (OnlyEnemyFilter)
		if (!g_NewPatch.DüsmanID[PlayerID] && !g_NewPatch.DüsmanDogrulama[PlayerID] && g_NewPatch.DüsmanName[PlayerID] != getPlayerName(PlayerID))
			return false;

	// FÝGHT
	if (PlayerHaveMyColor && samp_color_get(PlayerID) == samp_color_get(g_Players->sLocalPlayerID))
		return false;
	if (Ignore2Levels && g_Players->pRemotePlayer[PlayerID]->iScore < 2)
		return false;
	if (IgnoreDriver && g_Players->pRemotePlayer[PlayerID]->pPlayerData->bytePlayerState == PLAYER_STATE_DRIVER)
		return false;
	if (IgnoreOnfoot && g_Players->pRemotePlayer[PlayerID]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
		return false;
	if (IgnoreUnder10Levels && g_Players->pRemotePlayer[PlayerID]->iScore <= 10)
		return false;

	return true;

}

int GetAimingPlayer()
{
	if (g_SAMP == NULL) return -1;


	int targetid = -1;
	D3DXVECTOR3 ReversePosedion, ReverseEkranPozisyonu;
	CVector2D Screen;


	float  FFNeah = 0.0;

	for (int i = 0; i < 1004; i++)
	{

		if (AimingFiltersISSafe(i) != true)
			continue;

		float Posedion[3];
		getPlayerPos(i, Posedion);



		ReversePosedion.x = Posedion[0];
		ReversePosedion.y = Posedion[1];
		ReversePosedion.z = Posedion[2];


		CalcScreenCoors(&ReversePosedion, &ReverseEkranPozisyonu);

		if (ReverseEkranPozisyonu.z < 1.0f)
			continue;

		Screen.fX = ReverseEkranPozisyonu.x;
		Screen.fY = ReverseEkranPozisyonu.y;

		double GGNeah = sqrt(((GetSystemMetrics(SM_CXSCREEN) * 0.5299999714f - Screen.fX) *
			(GetSystemMetrics(SM_CXSCREEN) * 0.5299999714f - Screen.fX))
			 + ((GetSystemMetrics(SM_CYSCREEN) * 0.4f - Screen.fY) * (GetSystemMetrics(SM_CYSCREEN) * 0.4f - Screen.fY)));


		if (GGNeah > FFNeah && FFNeah != 0.0) continue;
	
		FFNeah = GGNeah;
		targetid = i;
	}

	return targetid;
}

float CalculateDamage(void)
{
	WORD gun = g_Players->pLocalPlayer->byteCurrentWeapon;
	float dmg = 0.0f;
	if (gun == 24 || gun == 38) dmg = 46.200000762939453125f;
	if (gun == 22 || gun == 29) dmg = 8.25f;
	if (gun == 23) dmg = 13.200000762939453125f;
	if (gun == 28 || gun == 32) dmg = 6.6000003814697265625f;
	if (gun == 30 || gun == 31) dmg = 9.90000057220458984375f;
	if (gun == 33) dmg = 24.7500019073486328125f;
	if (gun == 34) dmg = 41.25f;

	if (gun == 25 || gun == 26)
	{
		int shotrandom = rand() % 8 + 1;
		switch (shotrandom)
		{
		case 1:
			dmg = 26.4000015258789f;
			break;
		case 2:
			dmg = 29.70000171661377f;
			break;
		case 3:
			dmg = 33.00000190734863f;
			break;
		case 4:
			dmg = 36.30000209808349f;
			break;
		case 5:
			dmg = 39.60000228881836f;
			break;
		case 6:
			dmg = 42.90000247955322f;
			break;
		case 7:
			dmg = 46.20000267028808f;
			break;
		case 8:
			dmg = 49.50000286102295f;
			break;
		}
	}

	if (gun == 27)
	{
		int shotrandom2 = rand() % 4 + 1;
		switch (shotrandom2)
		{
		case 1:
			dmg = 24.75000143051147f;
			break;
		case 2:
			dmg = 29.70000171661376f;
			break;
		case 3:
			dmg = 34.65000200271606f;
			break;
		case 4:
			dmg = 39.60000228881835f;
			break;
		}
	}

	return dmg;
}

void CalcScreenCoorskyze(CVector* vecWorld, CVector* vecScreen)
{
	D3DXMATRIX m((float*)(0xB6FA2C));

	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	vecScreen->fX = (vecWorld->fZ * m._31) + (vecWorld->fY * m._21) + (vecWorld->fX * m._11) + m._41;
	vecScreen->fY = (vecWorld->fZ * m._32) + (vecWorld->fY * m._22) + (vecWorld->fX * m._12) + m._42;
	vecScreen->fZ = (vecWorld->fZ * m._33) + (vecWorld->fY * m._23) + (vecWorld->fX * m._13) + m._43;

	double fRecip = (double)1.0 / vecScreen->fZ;
	vecScreen->fX *= (float)(fRecip * (*dwLenX));
	vecScreen->fY *= (float)(fRecip * (*dwLenY));
}

/*                                                                                */

int isBadPtr_GTA_pPed(actor_info* pActorInfo)
{
	if (pActorInfo == NULL)
		return 1;
	if (!
		(
			(DWORD)pActorInfo >= (DWORD)pool_actor->start
			&& (DWORD)pActorInfo <= ((DWORD)pool_actor->start + (pool_actor->size * sizeof(actor_info)))
			)
		)
		return 1;
	return (pActorInfo->base.matrix == NULL) || (pActorInfo->base.nType != ENTITY_TYPE_PED);
}

struct actor_info* actor_info_get(int id, int flags)
{
	struct actor_info* info;

	if (pool_actor == NULL)
		return NULL;

	if (id != ACTOR_SELF && (id < 0 && id >= pool_actor->size))
		return NULL;

	if (id == ACTOR_SELF)
	{
		info = (struct actor_info*)(UINT_PTR) * (uint32_t*)ACTOR_POINTER_SELF;
		if (isBadPtr_GTA_pPed(info))
			return NULL;
	}
	else
	{
		info = &((struct actor_info*)pool_actor->start)[id];
		if (isBadPtr_GTA_pPed(info))
			return NULL;
	}

	/* XXX this should not be nessecary... but it is. fix it. */
	if (vect3_near_zero(&info->base.matrix[4 * 3]))
		return NULL;

	if ((flags & ACTOR_ALIVE) && ACTOR_IS_DEAD(info))
		return NULL;

	/* exclude actors in the same car as ACTOR_SELF */
	if (flags & ACTOR_NOT_SAME_VEHICLE)
	{
		struct actor_info* self = actor_info_get(ACTOR_SELF, 0);

		if (self != NULL
			&& info->state == ACTOR_STATE_DRIVING
			&& self->state == ACTOR_STATE_DRIVING
			&& info->vehicle == self->vehicle) return NULL;
	}

	return info;
}



int vehicle_contains_actor(struct vehicle_info* vehicle, struct actor_info* actor)
{
	struct actor_info* passengers[9];
	struct actor_info* temp;

	if (actor == NULL)
		return NULL;

	memcpy(passengers, vehicle->passengers, sizeof(uint32_t) * 9);

	int i;
	for (i = 0; i < 9; i++)
	{
		temp = passengers[i];
		if (temp == actor)
			return 1;
	}

	return NULL;
}

int isBadPtr_GTA_pPed(CPed* pCPed)
{
	if (pCPed == NULL)
		return 1;
	CPedSAInterface* pCPedSAInterface = pCPed->GetPedInterface();
	if (!
		(
			(DWORD)pCPedSAInterface >= (DWORD)pool_actor->start
			&& (DWORD)pCPedSAInterface <= ((DWORD)pool_actor->start + (pool_actor->size * sizeof(CPedSAInterface)))
			)
		)
		return 1;
	return (pCPedSAInterface->Placeable.matrix == NULL) || (pCPedSAInterface->nType != ENTITY_TYPE_PED);
}

int isBadPtr_GTA_pVehicle(vehicle_info* p_VehicleInfo)
{
	if (p_VehicleInfo == NULL)
		return 1;
	if (!
		(
			(DWORD)p_VehicleInfo >= (DWORD)pool_vehicle->start
			&& (DWORD)p_VehicleInfo <= ((DWORD)pool_vehicle->start + (pool_vehicle->size * sizeof(vehicle_info)))
			)
		)
		return 1;
	return (p_VehicleInfo->base.matrix == NULL) || (p_VehicleInfo->base.nType != ENTITY_TYPE_VEHICLE);
}

struct vehicle_info* vehicle_info_get(int id, int flags)
{
	struct vehicle_info* info;

	if (pool_vehicle == NULL)
		return NULL;

	if (id != VEHICLE_SELF && (id < 0 && id >= pool_vehicle->size))
		return NULL;

	if (id == VEHICLE_SELF)
	{
		info = (struct vehicle_info*)(UINT_PTR) * (uint32_t*)VEHICLE_POINTER_SELF;
		if (isBadPtr_GTA_pVehicle(info))
			return NULL;

		// check to see if we're actually in the vehicle
		struct actor_info* actor = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);
		if (!vehicle_contains_actor(info, actor))
			return NULL;
	}
	else
	{
		info = &((struct vehicle_info*)pool_vehicle->start)[id];
		if (isBadPtr_GTA_pVehicle(info))
			return NULL;
	}

	// filtering for VEHICLE_EMPTY
	// put it here if we ever actually use it =P
	if ((flags & VEHICLE_ALIVE) && (info->hitpoints < 250.0f && info->burn_timer == 0.0f))
		return NULL;

	return info;
}


bool IsChatOpened()
{
	if (g_Input->pDXUTEditBox->bIsChatboxOpen)
		return true;
	else
		return false;

}

bool IsPlayerFriend(uint16_t playerID)
{
	if (g_NewPatch.FriendID[playerID] && g_NewPatch.FriendDogrulama[playerID] && g_NewPatch.FriendName[playerID] == getPlayerName(playerID))
		return true;
	else
		return false;

}

bool IsPlayerEnemy(uint16_t playerID)
{
	if (playerID == g_Players->sLocalPlayerID)
		return false;
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;

	if (g_NewPatch.DüsmanID[playerID] && g_NewPatch.DüsmanDogrulama[playerID] && g_NewPatch.DüsmanName[playerID] == getPlayerName(playerID))
		return true;
	else
		return false;

}


bool IsVehicleStreamed(uint16_t vehicleID)
{

	if (g_Vehicles->iIsListed[vehicleID] != 1)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
		return false;

	return true;
}

bool IsPlayerConnected(uint16_t playerID)
{
	if (playerID == g_Players->sLocalPlayerID)
		return false;
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;

	return true;
}

bool IsPlayerStreamed(uint16_t playerID)
{
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;

	return true;
}

bool IsPlayerShot(int playerID)
{
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->onFootData.stSampKeys.keys_secondaryFire__shoot)
		return true;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->passengerData.stSampKeys.keys_secondaryFire__shoot)
		return true;

	return false;
}


bool IsPlayerInVehicle()
{
	struct vehicle_info* vehicle_info;
	vehicle_info = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);

	if (vehicle_info == nullptr)
		return false;

	return true;
}



/*                                                                                */

void CalcScreenCoors(D3DXVECTOR3* vecWorld, D3DXVECTOR3* vecScreen)
{

	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/
	// Get the static view matrix as D3DXMATRIX
	D3DXMATRIX	m((float*)(0xB6FA2C));

	// Get the static virtual screen (x,y)-sizes
	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	//DWORD *dwLenZ = (DWORD*)(0xC1704C);
	//double aspectRatio = (*dwLenX) / (*dwLenY);
	// Do a transformation
	vecScreen->x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen->y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen->z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	// Get the correct screen coordinates
	double	fRecip = (double)1.0 / vecScreen->z;	//(vecScreen->z - (*dwLenZ));
	vecScreen->x *= (float)(fRecip * (*dwLenX));
	vecScreen->y *= (float)(fRecip * (*dwLenY));
}

bool isInsideCircle(float circle_x, float circle_y, float rad, float x, float y)
{
	if ((x - circle_x) * (x - circle_x) +
		(y - circle_y) * (y - circle_y) <= rad * rad)
		return true;
	else
		return false;
}

DWORD rainbow()
{

	static float x = 0, y = 0;
	static float r = 1, g = 1, b = 1;

	if (y >= 0.0f && y < 255.0f) {
		r = 255.0f;
		g = 0.0f;
		b = x;
	}
	else if (y >= 255.0f && y < 510.0f) {
		r = 255.0f - x;
		g = 0.0f;
		b = 255.0f;
	}
	else if (y >= 510.0f && y < 765.0f) {
		r = 0.0f;
		g = x;
		b = 255.0f;
	}
	else if (y >= 765.0f && y < 1020.0f) {
		r = 0.0f;
		g = 255.0f;
		b = 255.0f - x;
	}
	else if (y >= 1020.0f && y < 1275.0f) {
		r = x;
		g = 255.0f;
		b = 0.0f;
	}
	else if (y >= 1275.0f && y < 1530.0f) {
		r = 255.0f;
		g = 255.0f - x;
		b = 0.0f;
	}

	x += 255.0f;
	if (x >= 255.0f)
		x = 0.0f;

	y += 0.450f;
	if (y > 1530.0f)
		y = 0.0f;


	return D3DCOLOR_ARGB(255, (int)r, (int)g, (int)b);
}

D3DCOLOR RandomColorz()
{
	int one = rand() % 256;
	int two = rand() % 256;
	int three = rand() % 256;

	D3DCOLOR color = D3DCOLOR_ARGB(255, one, two, three);

	return color;
}

void PressKey(DWORD Key)
{
	// ...
	INPUT ip;
	// ...
		// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press the "A" key
	ip.ki.wVk = Key; // virtual-key code for the "a" keyKey
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
}

DWORD FindProcessId(const std::string& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

void DisembarkInstantly()
{
	CTaskManager* taskManager = pPedSelf->GetPedIntelligence()->GetTaskManager();
	for (int i = 0; i < TASK_PRIORITY_MAX; i++)
	{
		CTask* pTask = taskManager->GetTask(i);
		if (pTask)
		{
			pTask->MakeAbortable(pPedSelf, ABORT_PRIORITY_IMMEDIATE, NULL);
			taskManager->RemoveTask(i);
		}
	}
	for (int i = 0; i < TASK_SECONDARY_MAX; i++)
	{
		CTask* pTask = taskManager->GetTaskSecondary(i);
		if (pTask)
		{
			pTask->MakeAbortable(pPedSelf, ABORT_PRIORITY_IMMEDIATE, NULL);
			if (i != TASK_SECONDARY_FACIAL_COMPLEX)
				taskManager->RemoveTaskSecondary(i);
		}
	}
	
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	return buf;
}

void gta_weather_state_set(int state)
{
	*(uint16_t*)0x00C81318 = (uint16_t)state;
	*(uint16_t*)0x00C8131C = (uint16_t)state;
	*(uint16_t*)0x00C81320 = (uint16_t)state;
}

void gta_time_hour_set(int hour)
{
	*(uint8_t*)0x00B70153 = (uint8_t)hour;
}

void gta_money_set(uint32_t value)
{
	*(uint32_t*)0x00B7CE50 = value;
	*(uint32_t*)0x00B7CE54 = value;
}

uint32_t gta_money_get(void)
{
	return *(uint32_t*)0x00B7CE50;
}

void gta_gravity_set(float value)
{
	memcpy_safe((void*)0x00863984, &value, sizeof(float));
}

float gta_gravity_get(void)
{
	float	value;

	memcpy_safe(&value, (void*)0x00863984, sizeof(float));

	return value;
}

void gta_game_speed_set(float value)
{
	*(float*)0x00B7CB64 = value;
}

float gta_game_speed_get(void)
{
	return *(float*)0x00B7CB64;
}

int gta_menu_active(void)
{
	return (int)(*(uint8_t*)0x00BA67A4);
}

void gta_menu_active_set(int enabled)
{
	/* untested */
	*(uint8_t*)0x00BA67A4 = (uint8_t)enabled;
}

struct checkpoint* gta_checkpoint_info_get(int n)
{
	struct checkpoint* cp = (struct checkpoint*)0x00C7F158;

	if (vect3_near_zero(cp->position))
		return NULL;

	return &cp[n];
}

void cheat_teleport(const float pos[3], int interior_id)
{
	if (player_state == CHEAT_STATE_ACTOR)
		cheat_actor_teleport(actor_info_get(ACTOR_SELF, 0), pos, interior_id);
}

void GTAfunc_CameraOnActor(actor_info* actor)
{
	if(actor == NULL) return;
	
	CEntity* pEntity = pGameInterface->GetPools()->GetEntity((DWORD*)actor);
	CCamera* pCamera = pGameInterface->GetCamera();
	if (pCamera && pEntity)
		pCamera->TakeControl(pEntity, MODE_FOLLOWPED, 1);
		
}

void GTAfunc_CameraOnVehicle(vehicle_info* vehicle)
{
	if (vehicle == NULL) return;

	
	CEntity* pEntity = pGameInterface->GetPools()->GetEntity((DWORD*)vehicle);
	CCamera* pCamera = pGameInterface->GetCamera();
	if (pCamera && pEntity)
		pCamera->TakeControl(pEntity, MODE_BEHINDCAR, 1);
		
}


void spectateHandle(void)
{
	if (g_iSpectateEnabled)
	{
		if (g_iSpectateLock) return;

		if (g_iSpectatePlayerID != -1)
		{
			if (g_Players->iIsListed[g_iSpectatePlayerID] != 0)
			{
				if (g_Players->pRemotePlayer[g_iSpectatePlayerID] != NULL)
				{
					int iState = getPlayerState(g_iSpectatePlayerID);

					if (iState == PLAYER_STATE_ONFOOT)
					{
						struct actor_info* pPlayer = getGTAPedFromSAMPPlayerID(g_iSpectatePlayerID);
						if (pPlayer == NULL) return;
						GTAfunc_CameraOnActor(pPlayer);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_DRIVER)
					{
						struct vehicle_info* pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_PASSENGER)
					{
						struct vehicle_info* pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
				}
				else
				{

					g_iSpectateEnabled = 0;
				}
			}
		}
	}
}

void spectatePlayer(int iPlayerID)
{
	if (iPlayerID == -1) return;

	g_iSpectatePlayerID = iPlayerID;
	g_iSpectateLock = 0;
	g_iSpectateEnabled = 1;
}

void spectatePlayeroff(int iPlayerID)
{
	if (iPlayerID == -1) return;

	pGameInterface->GetCamera()->RestoreWithJumpCut();

	g_iSpectatePlayerID = iPlayerID;
	g_iSpectateLock = 0;
	g_iSpectateEnabled = 0;
}

void cheat_actor_teleport(struct actor_info* info, const float pos[3], int interior_id)
{
	if (info == NULL)
		return;

	vect3_zero(info->speed);
	vect3_copy(pos, &info->base.matrix[4 * 3]);
}

int gta_weapon_ammo_set(struct actor_info* info, int slot, int ammo)
{
	uint32_t	ammo_old = info->weapon[slot].ammo;

	if (ammo >= 0)
		info->weapon[slot].ammo = ammo;

	return ammo_old;
}

int gta_weapon_ammo_clip_set(struct actor_info* info, int slot, int ammo_clip)
{
	uint32_t	ammo_clip_old = info->weapon[slot].ammo_clip;

	if (ammo_clip >= 0)
		info->weapon[slot].ammo_clip = ammo_clip;

	return ammo_clip_old;
}

int GTAfunc_isModelLoaded(int iModelID)
{
	int* ModelsLoadStateArray = (int*)0x8E4CD0;
	return ModelsLoadStateArray[5 * iModelID];
}

void GTAfunc_requestModelLoad(int iModelID)
{
	if (iModelID < 0)
		return;

	__asm
	{
		push 2
		push iModelID
		mov edx, FUNC_RequestModel
		call edx
		pop edx
		pop edx
	}
}

void GTAfunc_loadRequestedModels(void)
{
	uint32_t	func_load = 0x40ea10;
	__asm
	{
		push 0
		call func_load
		add esp, 4
	}
}

void gta_jetpack_give(void)
{
	typedef void* (__cdecl* jetpack_give_func) (void);
	jetpack_give_func	jetpack_give = (jetpack_give_func)0x00439600;

	jetpack_give();

}

int gta_interior_id_get(void)
{
	return (int)*(uint32_t*)0x00B72914;
}

void gta_interior_id_set(int id)
{
	struct actor_info* info = actor_info_get(ACTOR_SELF, 0);

	*(uint32_t*)0x00B72914 = (uint32_t)id;

	if (info != NULL)
		info->base.interior_id = (uint8_t)id;
}

void gta_blow_all_cars()
{
	uint32_t	func = 0x439D80;
	__asm call func
}

void SetCloudsEnabled(int iEnabled)
{
	//volumetric clouds
	if (iEnabled)
		memcpy_safe((void*)0x716380, "\xA1", 1);
	else
		memcpy_safe((void*)0x716380, "\xC3", 1);

	// foggy weather clouds (above ground)
	if (iEnabled)
		memcpy_safe((void*)0x716C90, "\xD9", 1);
	else
		memcpy_safe((void*)0x716C90, "\xC3", 1);

	// normal clouds
	//0071395A     90             NOP
	if (iEnabled)
		memcpy_safe((void*)0x713950, "\x83", 1);
	else
		memcpy_safe((void*)0x713950, "\xC3", 1);

	// plane trails (not really clouds, but they're sort of vapour)
	if (iEnabled)
		memcpy_safe((void*)0x717180, "\x83\xEC\x08", 3);
	else
		memcpy_safe((void*)0x717180, "\xC2\x04\x00", 3);
}


int vehicle_getColor0(vehicle_info* vinfo)
{
	if (vinfo == NULL)
		return -1;
	return vinfo->color[0];
}

int vehicle_getColor1(vehicle_info* vinfo)
{
	if (vinfo == NULL)
		return -1;
	return vinfo->color[1];
}

void vehicle_setColor0(vehicle_info* vinfo, int new_color)
{

	if (vinfo == NULL)
		return;

	vinfo->color[0] = new_color;
	if (g_SAMP != NULL)
	{
		uint16_t sampVeh = g_Players->pLocalPlayer->sCurrentVehicleID;
		if (!isBadSAMPVehicleID(sampVeh))
		{
			sendSCMEvent(3, sampVeh, new_color, vinfo->color[1]);
			if (g_Vehicles->pSAMP_Vehicle[sampVeh] != nullptr)
			{
				g_Vehicles->pSAMP_Vehicle[sampVeh]->byteColor[0] = new_color;
			}
		}
	}
}

void vehicle_setColor1(vehicle_info* vinfo, int new_color)
{

	if (vinfo == NULL)
		return;

	vinfo->color[1] = new_color;
	if (g_SAMP != NULL)
	{
		uint16_t sampVeh = g_Players->pLocalPlayer->sCurrentVehicleID;
		if (!isBadSAMPVehicleID(sampVeh))
		{
			sendSCMEvent(3, sampVeh, vinfo->color[0], new_color);
			if (g_Vehicles->pSAMP_Vehicle[sampVeh] != nullptr)
			{
				g_Vehicles->pSAMP_Vehicle[sampVeh]->byteColor[1] = new_color;
			}
		}
	}
}


/*                                                                                */

bool isBadPtr_handlerAny(void* pointer, ULONG size, DWORD dwFlags)
{
	DWORD						dwSize;
	MEMORY_BASIC_INFORMATION	meminfo;

	if (NULL == pointer)
		return true;

	memset(&meminfo, 0x00, sizeof(meminfo));
	dwSize = VirtualQuery(pointer, &meminfo, sizeof(meminfo));

	if (0 == dwSize)
		return true;

	if (MEM_COMMIT != meminfo.State)
		return true;

	if (0 == (meminfo.Protect & dwFlags))
		return true;

	if (size > meminfo.RegionSize)
		return true;

	if ((unsigned)((char*)pointer - (char*)meminfo.BaseAddress) > (unsigned)(meminfo.RegionSize - size))
		return true;

	return false;
}

bool isBadPtr_readAny(void* pointer, ULONG size)
{
	return isBadPtr_handlerAny(pointer, size, PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
		PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
}

bool isBadPtr_writeAny(void* pointer, ULONG size)
{
	return isBadPtr_handlerAny(pointer, size,
		PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
}

static int __page_size_get(void)
{
	static int	page_size = -1;
	SYSTEM_INFO si;

	if (page_size == -1)
	{
		GetSystemInfo(&si);
		page_size = (int)si.dwPageSize;
	}

	return page_size;
}

static int __page_write(void* _dest, const void* _src, uint32_t len)
{
	static int		page_size = __page_size_get();
	uint8_t* dest = (uint8_t*)_dest;
	const uint8_t* src = (const uint8_t*)_src;
	DWORD			prot_prev = 0;
	int				prot_changed = 0;
	int				ret = 1;

	while (len > 0)
	{
		ret = 1;
		int page_offset = (int)((UINT_PTR)dest % page_size);
		int page_remain = page_size - page_offset;
		int this_len = len;

		if (this_len > page_remain)
			this_len = page_remain;

		if (isBadPtr_writeAny(dest, this_len))
		{
			if (!VirtualProtect((void*)dest, this_len, PAGE_EXECUTE_READWRITE, &prot_prev))
				ret = 0;
			else
				prot_changed = 1;
		}

		if (ret)
			memcpy(dest, src, this_len);

		if (prot_changed)
		{
			DWORD	dummy;
			if (!VirtualProtect((void*)dest, this_len, prot_prev, &dummy))
				return 0;
		}

		dest += this_len;
		src += this_len;
		len -= this_len;
	}

	return ret;
}

static int __page_read(void* _dest, const void* _src, uint32_t len)
{
	static int	page_size = __page_size_get();
	uint8_t* dest = (uint8_t*)_dest;
	uint8_t* src = (uint8_t*)_src;
	DWORD		prot_prev = 0;
	int			prot_changed = 0;
	int			ret = 1;

	while (len > 0)
	{
		ret = 1;
		int page_offset = (int)((UINT_PTR)src % page_size);
		int page_remain = page_size - page_offset;
		int this_len = len;

		if (this_len > page_remain)
			this_len = page_remain;

		if (isBadPtr_readAny(src, this_len))
		{
			if (!VirtualProtect((void*)src, this_len, PAGE_EXECUTE_READWRITE, &prot_prev))
				ret = 0;
			else
				prot_changed = 1;
		}

		if (ret)
			memcpy(dest, src, this_len);
		else
			memset(dest, 0, this_len);

		if (prot_changed)
		{
			DWORD	dummy;
			if (!VirtualProtect((void*)src, this_len, prot_prev, &dummy))
				return 0;
		}

		dest += this_len;
		src += this_len;
		len -= this_len;
	}

	return ret;
}

int memcpy_safe(void* _dest, const void* _src, uint32_t len, int check, const void* checkdata)
{
	static int		page_size = __page_size_get();
	static int		recurse_ok = 1;
	uint8_t			buf[4096];
	uint8_t* dest = (uint8_t*)_dest;
	const uint8_t* src = (const uint8_t*)_src;
	int				ret = 1;

	if (check && checkdata)
	{
		if (!memcmp_safe(checkdata, _dest, len))
			return 0;
	}

	while (len > 0)
	{
		uint32_t	this_len = sizeof(buf);

		if (this_len > len)
			this_len = len;

		if (!__page_read(buf, src, this_len))
			ret = 0;

		if (!__page_write(dest, buf, this_len))
			ret = 0;

		len -= this_len;
		src += this_len;
		dest += this_len;
	}

	return ret;
}

int memset_safe(void* _dest, int c, uint32_t len)
{
	uint8_t* dest = (uint8_t*)_dest;
	uint8_t buf[4096];

	memset(buf, c, (len > 4096) ? 4096 : len);

	for (;; )
	{
		if (len > 4096)
		{
			if (!memcpy_safe(dest, buf, 4096))
				return 0;
			dest += 4096;
			len -= 4096;
		}
		else
		{
			if (!memcpy_safe(dest, buf, len))
				return 0;
			break;
		}
	}

	return 1;
}

int memcmp_safe(const void* _s1, const void* _s2, uint32_t len)
{
	const uint8_t* s1 = (const uint8_t*)_s1;
	const uint8_t* s2 = (const uint8_t*)_s2;
	uint8_t			buf[4096];

	for (;; )
	{
		if (len > 4096)
		{
			if (!memcpy_safe(buf, s1, 4096))
				return 0;
			if (memcmp(buf, s2, 4096))
				return 0;
			s1 += 4096;
			s2 += 4096;
			len -= 4096;
		}
		else
		{
			if (!memcpy_safe(buf, s1, len))
				return 0;
			if (memcmp(buf, s2, len))
				return 0;
			break;
		}
	}

	return 1;
}

void* dll_baseptr_get(const char* dll_name)
{
	return GetModuleHandle(dll_name);
}

size_t strlcpy(char* dst, const char* src, size_t size)
{
	size_t	len = strlen(src);

	if (size == 0)
		return len;

	if (len >= size)
	{
		size--;
		memcpy(dst, src, size);
		dst[size] = 0;
	}
	else if (size > 0)
	{
		strcpy(dst, src);
	}

	return len;
}

size_t strlcat(char* dst, const char* src, size_t size)
{
	size_t	dlen = strlen(dst);
	size_t	slen = strlen(src);

	if (size == 0)
		return dlen + slen;

	if (dlen + slen >= size)
	{
		size -= dlen - 1;
		memcpy(dst + dlen, src, size);
		dst[dlen + size] = 0;
	}
	else if (size > 0)
	{
		strcpy(dst + dlen, src);
	}

	return dlen + slen;
}

bool findstrinstr(char* text, char* find)
{
	char	realtext[256];
	char	subtext[256];
	char* result;
	char* next;
	char	temp;
	int		i = 0;


	// can't find stuff that isn't there unless you are high
	if (text == NULL || find == NULL)
		return false;

	// lower case text ( sizeof()-2 = 1 for array + 1 for termination after while() )
	while (text[i] != NULL && i < (sizeof(realtext) - 2))
	{
		temp = text[i];
		if (isupper(temp))
			temp = tolower(temp);
		realtext[i] = temp;
		i++;
	}
	realtext[i] = 0;

	// replace unwanted characters/spaces with dots
	i = 0;
	while (find[i] != NULL && i < (sizeof(subtext) - 2))
	{
		temp = find[i];
		if (isupper(temp))
			temp = tolower(temp);
		if (!isalpha(temp))
			temp = '.';
		subtext[i] = temp;
		i++;
	}
	subtext[i] = 0;

	// use i to count the successfully found text parts
	i = 0;

	// split and find every part of subtext/find in text
	result = &subtext[0];
	while (*result != NULL)
	{
		next = strstr(result, ".");
		if (next != NULL)
		{
			// more than one non-alphabetic character
			if (next == result)
			{
				do
					next++;
				while (*next == '.');

				if (*next == NULL)
					return (i != 0);
				result = next;
				next = strstr(result, ".");
				if (next != NULL)
					*next = NULL;
			}
			else
				*next = NULL;
		}

		if (strstr(realtext, result) == NULL)
			return 0;

		if (next == NULL)
			return true;

		i++;
		result = next + 1;
	}

	return false;
}

void* memdup(const void* src, int len)
{
	void* dest = malloc(len);

	if (dest != NULL)
		memcpy(dest, src, len);

	return dest;
}

D3DXVECTOR3 CVecToD3DXVEC(CVector vec)
{
	return D3DXVECTOR3(vec.fX, vec.fY, vec.fZ);
}

/*                                                                                */
