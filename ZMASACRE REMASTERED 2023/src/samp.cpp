/*

     ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#include "main.h"

#define SAMP_DLL				"samp.dll"
#define SAMP_CMP03DL			"528D44240C508D7E09E8"

// global samp pointers
int								g_renderSAMP_initSAMPstructs = 0;

stSAMP*							g_SAMP = nullptr;
stPlayerPool*					g_Players = nullptr;
stVehiclePool*					g_Vehicles = nullptr;
stChatInfo*						g_Chat = nullptr;
stInputInfo*					g_Input = nullptr;
stKillInfo*						g_DeathList = nullptr;
stScoreboardInfo*				g_Scoreboard = nullptr;
stDialogInfo*					g_DialogInfo = nullptr;

stNewPatch                      g_NewPatch;
stStreamedOutPlayerInfo			g_stStreamedOutInfo;



//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// FUNCTIONS //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int getVehicleGTAIDFromInterface(DWORD* vehicle)
{
	return (int)((DWORD)vehicle - (DWORD)pool_vehicle->start) / 2584;
}

void update_translateGTASAMP_vehiclePool(void)
{
	if (!g_Vehicles)
		return;

	int iGTAID;
	for (int i = 0; i <= SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i] == nullptr)
			continue;
		if (g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle == nullptr)
			continue;
		/*if (isBadPtr_writeAny(g_Vehicles->pSAMP_Vehicle[i], sizeof(stSAMPVehicle)))
			continue;*/
		iGTAID = getVehicleGTAIDFromInterface((DWORD*)g_Vehicles->pSAMP_Vehicle[i]->pGTA_Vehicle);
		if (iGTAID <= SAMP_MAX_VEHICLES && iGTAID >= 0)
		{
			translateGTASAMP_vehiclePool.iSAMPID[iGTAID] = i;
		}
	}
}

int getPedGTAIDFromInterface(DWORD* ped)
{
	return (int)((DWORD)ped - (DWORD)pool_actor->start) / 1988;
}

void update_translateGTASAMP_pedPool(void)
{
	if (!g_Players)
		return;

	int iGTAID, i;
	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (i == g_Players->sLocalPlayerID)
		{
			translateGTASAMP_pedPool.iSAMPID[0] = i;
			continue;
		}
		if (g_Players->pRemotePlayer[i] == nullptr)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData == nullptr)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == nullptr)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTAEntity == nullptr)
			continue;

		iGTAID = getPedGTAIDFromInterface((DWORD*)g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTAEntity);
		if (iGTAID <= SAMP_MAX_PLAYERS && iGTAID >= 0)
		{
			translateGTASAMP_pedPool.iSAMPID[iGTAID] = i;
		}
	}
}


static signed char hex_to_dec(signed char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'A' + 10;

	return -1;
}

uint8_t* hex_to_bin(const char* str)
{
	int		len = (int)strlen(str);
	uint8_t* buf, * sbuf;

	if (len == 0 || len % 2 != 0)
		return NULL;

	sbuf = buf = (uint8_t*)malloc(len / 2);

	while (*str)
	{
		signed char bh = hex_to_dec(*str++);
		signed char bl = hex_to_dec(*str++);

		if (bl == -1 || bh == -1)
		{
			free(sbuf);
			return NULL;
		}

		*buf++ = (uint8_t)(bl | (bh << 4));
	}

	return sbuf;
}

bool hex_is_valid(const std::string& hex)
{
	if (hex.empty())
		return false;
	for (size_t i = 0, len = hex.length(); i < len; i++)
	{
		if (hex_to_dec(hex[i]) == -1)
			return false;
	}
	return true;
}

D3DCOLOR hex_to_color(const char* str, int len)
{
	char buf[12];
	strncpy_s(buf, str, len);
	D3DCOLOR color = 0x00;
	byte* colorByteSet = (byte*)&color;
	int stri = 0;
	for (int i = sizeof(color) - 1; i >= 0; i--)
	{
		if (i == 3 && len == 6)
		{
			colorByteSet[3] = 0xFF;
		}
		else
		{
			signed char bh = hex_to_dec(buf[stri++]);
			signed char bl = hex_to_dec(buf[stri++]);
			if (bh != -1 && bl != -1)
			{
				colorByteSet[i] = bl | (bh << 4);
			}
		}
	}
	return color;
}

void sampPatchDisableAnticheat(void)
{
	struct patch_set fuckAC =
	{
		"kyenub patch", 0, 0,
		{
			{ 1, (void*)(g_dwSAMP_Addr + 0x9D6F0), NULL, (uint8_t*)"\xC3", 0 },
			{ 8, (void*)(g_dwSAMP_Addr + 0xC5C10), NULL, (uint8_t*)"\xB8\x45\x00\x00\x00\xC2\x1C\x00", 0 },
		}
	};
	patcher_install(&fuckAC);

	static uint32_t anticheat = 1;
	byte acpatch[] = { 0xFF, 0x05, 0x00, 0x00, 0x00, 0x00, 0xA1, 0x00, 0x00, 0x00, 0x00, 0xC3 };
	*(uint32_t**)&acpatch[2] = *(uint32_t**)&acpatch[7] = &anticheat;
	memcpy_safe((void*)(g_dwSAMP_Addr + 0xC5E2B), acpatch, 12);
}


void getSamp()
{

	uint32_t	samp_dll = getSampAddress();

	if (samp_dll != NULL)
	{
		g_dwSAMP_Addr = (uint32_t)samp_dll;

		if (g_dwSAMP_Addr != NULL)
		{
			if (memcmp_safe((uint8_t*)g_dwSAMP_Addr + 0xBABE, hex_to_bin(SAMP_CMP03DL), 10))
				sampPatchDisableAnticheat();		
			else
				g_dwSAMP_Addr = NULL;
		}
	}
}

uint32_t getSampAddress()
{


	uint32_t	samp_dll;


	samp_dll = (uint32_t)LoadLibrary(SAMP_DLL);


	return samp_dll;
}

template<typename T>
T GetSAMPPtrInfo(uint32_t offset)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;
	return *(T*)(g_dwSAMP_Addr + offset);
}

struct stSAMP* stGetSampInfo(void)
{
	return GetSAMPPtrInfo<stSAMP*>(SAMP_INFO_OFFSET);
}

struct stChatInfo* stGetSampChatInfo(void)
{
	return GetSAMPPtrInfo<stChatInfo*>(SAMP_CHAT_INFO_OFFSET);
}

struct stDialogInfo* stGetDialogInfo(void)
{
	return GetSAMPPtrInfo<stDialogInfo*>(SAMP_DIALOG_INFO_OFFSET);
}

struct stInputInfo* stGetInputInfo(void)
{
	return GetSAMPPtrInfo<stInputInfo*>(SAMP_CHAT_INPUT_INFO_OFFSET);
}

struct stKillInfo* stGetKillInfo(void)
{
	return GetSAMPPtrInfo<stKillInfo*>(SAMP_KILL_INFO_OFFSET);
}

struct stScoreboardInfo* stGetScoreboardInfo(void)
{
	return GetSAMPPtrInfo<stScoreboardInfo*>(SAMP_SCOREBOARD_INFO);
}

int isBadSAMPVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL || iVehicleID == (uint16_t)-1 || iVehicleID >= SAMP_MAX_VEHICLES)
		return 1;
	return !g_Vehicles->iIsListed[iVehicleID];
}

int isBadSAMPPlayerID(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return 1;
	return !g_Players->iIsListed[iPlayerID];
}

D3DCOLOR samp_color_get(int id, DWORD trans)
{
	if (g_dwSAMP_Addr == NULL)
		return NULL;

	D3DCOLOR* color_table;
	if (id < 0 || id >= (SAMP_MAX_PLAYERS + 3))
		return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);

	switch (id)
	{
	case (SAMP_MAX_PLAYERS):
		return 0xFF888888;

	case (SAMP_MAX_PLAYERS + 1):
		return 0xFF0000AA;

	case (SAMP_MAX_PLAYERS + 2):
		return 0xFF63C0E2;
	}

	color_table = (D3DCOLOR*)((uint8_t*)g_dwSAMP_Addr + SAMP_COLOR_OFFSET);
	return (color_table[id] >> 8) | trans;
}

int getNthPlayerID(int n)
{
	if (g_Players == NULL)
		return -1;

	int thisplayer = 0;
	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->sLocalPlayerID == i)
			continue;
		if (thisplayer < n)
		{
			thisplayer++;
			continue;
		}

		return i;
	}

	//shouldnt happen
	return -1;
}

int getPlayerCount(void)
{
	if (g_Players == NULL)
		return NULL;

	int iCount = 0;
	int i;

	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		iCount++;
	}

	return iCount + 1;
}

int setLocalPlayerName(const char* name)
{
	if (g_Players == NULL || g_Players->pLocalPlayer == NULL)
		return 0;


	int strlen_name = strlen(name);
	if (strlen_name == 0 || strlen_name > SAMP_ALLOWED_PLAYER_NAME_LENGTH)
		return 0;

	((void(__thiscall*) (void*, const char* name, int len)) (g_dwSAMP_Addr + SAMP_FUNC_NAMECHANGE)) (&g_Players->pVTBL_txtHandler, name, strlen_name);
	return 1;
}

int getVehicleCount(void)
{
	if (g_Vehicles == NULL)
		return NULL;

	int iCount = 0;
	int i;

	for (i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		iCount++;
	}

	return iCount;
}

int getPlayerPos(int iPlayerID, float fPos[3])
{
	struct actor_info* pActor = NULL;
	struct vehicle_info* pVehicle = NULL;

	struct actor_info* pSelfActor = actor_info_get(ACTOR_SELF, 0);

	if (g_Players == NULL)
		return 0;
	if (g_Players->iIsListed[iPlayerID] != 1)
		return 0;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return 0;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return 0;

	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return 0;	// not streamed
	else
	{
		pActor = g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped;

		if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle != NULL)
			pVehicle = g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;

		if (pVehicle != NULL && pActor->vehicle == pVehicle && pVehicle->passengers[0] == pActor)
		{
			// driver of a vehicle
			vect3_copy(&pActor->vehicle->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fVehiclePosition, fPos);
		}
		else if (pVehicle != NULL)
		{
			// passenger of a vehicle
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
		else
		{
			// on foot
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);

			//vect3_copy(g_Players->pRemotePlayer[iPlayerID]->fActorPosition, fPos);
		}
	}

	if (pSelfActor != NULL)
	{
		if (vect3_dist(&pSelfActor->base.matrix[4 * 3], fPos) < 100.0f)
			vect3_copy(&pActor->base.matrix[4 * 3], fPos);
	}

	// detect zombies
	if (vect3_near_zero(fPos))
		vect3_copy(&pActor->base.matrix[4 * 3], fPos);

	return !vect3_near_zero(fPos);
}

const char* getPlayerName(int iPlayerID)
{
	if (g_Players == NULL)
		return NULL;

	if (iPlayerID == g_Players->sLocalPlayerID)
		return g_Players->strLocalPlayerName.c_str();

	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;

	if (g_Players->pRemotePlayer[iPlayerID]->iNameAllocated <= 0xF)
		return g_Players->pRemotePlayer[iPlayerID]->strPlayerName.c_str();


	return g_Players->pRemotePlayer[iPlayerID]->strPlayerName.c_str();
}

int getPlayerState(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;
	if (iPlayerID == g_Players->sLocalPlayerID)
		return NULL;
	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->bytePlayerState;
}

int getPlayerSAMPVehicleID(int iPlayerID)
{
	if (g_Players == NULL && g_Vehicles == NULL) return 0;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL) return 0;
	if (g_Vehicles->pSAMP_Vehicle[g_Players->pRemotePlayer[iPlayerID]->pPlayerData->sVehicleID] == NULL) return 0;
	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->sVehicleID;
}

struct actor_info* getGTAPedFromSAMPPlayerID(int iPlayerID)
{
	if (g_Players == NULL || iPlayerID < 0 || iPlayerID > SAMP_MAX_PLAYERS)
		return NULL;
	if (iPlayerID == g_Players->sLocalPlayerID)
		return actor_info_get(ACTOR_SELF, 0);
	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped == NULL)
		return NULL;

	// return actor_info, null or otherwise
	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped;
}

struct vehicle_info* getGTAVehicleFromSAMPVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL || iVehicleID < 0 || iVehicleID >= SAMP_MAX_VEHICLES)
		return NULL;
	if (iVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		return vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);
	if (g_Vehicles->iIsListed[iVehicleID] != 1)
		return NULL;

	// return vehicle_info, null or otherwise
	return g_Vehicles->pGTA_Vehicle[iVehicleID];
}

int getSAMPPlayerIDFromGTAPed(struct actor_info* pGTAPed)
{
	if (g_Players == NULL)
		return 0;
	if (actor_info_get(ACTOR_SELF, 0) == pGTAPed)
		return g_Players->sLocalPlayerID;

	int i;
	for (i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (g_Players->iIsListed[i] != 1)
			continue;
		if (g_Players->pRemotePlayer[i] == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped == NULL)
			continue;
		if (g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor->pGTA_Ped == pGTAPed)
			return i;
	}

	return ACTOR_SELF;
}

int getSAMPVehicleIDFromGTAVehicle(struct vehicle_info* pVehicle)
{
	if (g_Vehicles == NULL)
		return NULL;
	if (vehicle_info_get(VEHICLE_SELF, 0) == pVehicle && g_Players != NULL)
		return g_Players->pLocalPlayer->sCurrentVehicleID;

	int i, iReturn = 0;
	for (i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pGTA_Vehicle[i] == pVehicle)
			return i;
	}

	return VEHICLE_SELF;
}

uint32_t getPedGTAScriptingIDFromPlayerID(int iPlayerID)
{
	if (g_Players == NULL)
		return NULL;

	if (g_Players->iIsListed[iPlayerID] != 1)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID] == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData == NULL)
		return NULL;
	if (g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor == NULL)
		return NULL;

	return g_Players->pRemotePlayer[iPlayerID]->pPlayerData->pSAMP_Actor->ulGTAEntityHandle;
}

uint32_t getVehicleGTAScriptingIDFromVehicleID(int iVehicleID)
{
	if (g_Vehicles == NULL)
		return NULL;

	if (g_Vehicles->iIsListed[iVehicleID] != 1)
		return NULL;
	if (g_Vehicles->pSAMP_Vehicle[iVehicleID] == NULL)
		return NULL;

	return g_Vehicles->pSAMP_Vehicle[iVehicleID]->ulGTAEntityHandle;
}

void addClientCommand(char* name, CMDPROC function)
{
	if (name == NULL || function == NULL || g_Input == NULL)
		return;

	if (g_Input->iCMDCount == (SAMP_MAX_CLIENTCMDS - 1))
		return;


	if (strlen(name) > 30)
		return;

	((void(__thiscall*) (void* _this, char* command, CMDPROC function)) (g_dwSAMP_Addr + SAMP_FUNC_ADDCLIENTCMD)) (g_Input, name, function);
}


void addMessage(int param, const char* text, ...)
{
	if (g_SAMP != NULL)
	{
		va_list ap;
		if (text == NULL)
			return;

		char	tmp[512];
		memset(tmp, 0, 512);

		va_start(ap, text);
		vsnprintf(tmp, sizeof(tmp) - 1, text, ap);
		va_end(ap);

		//#9c252d kýrmýzý
		char	buf[2048];
		if (param == 0)
		{
			snprintf(buf, sizeof(buf), "ZMasacre » Hata » {ffffff}%s", tmp);
			addToChatWindow(buf, D3DCOLOR_ARGB(255, 255, 000, 000));
		}
		else if(param == 1)
		{
			snprintf(buf, sizeof(buf), "ZMasacre » {ffffff}%s", tmp);
			addToChatWindow(buf, D3DCOLOR_ARGB(255, 000, 255, 000));
		}
		else if (param == 2)
		{
			snprintf(buf, sizeof(buf), "ZMasacre » Info » {ffffff}%s", tmp);
			addToChatWindow(buf, D3DCOLOR_ARGB(255, 255, 125, 000));
			
		}


	}
}

void Show_Dialog(UINT16 wDialogID, UINT8 bDialogStyle, char* szTitle, char* szButton1, char* szButton2, char* szInfo)
{
	if (g_SAMP == NULL) return;

	{
		void* Connection = *(void**)(g_dwSAMP_Addr + SAMP_DIALOG_INFO_OFFSET);
		((void(__thiscall*)
			(void*, int, int, char*, char*, char*, char*)) (g_dwSAMP_Addr + SAMP_DIALOG_SHOW_OFFSET))(Connection, wDialogID, bDialogStyle, szTitle, szButton1, szButton2, szInfo);
	}

}

void addToChatWindow(char* text, D3DCOLOR textColor, int playerID)
{
	if (g_SAMP == NULL || g_Chat == NULL)
		return;

	if (text == NULL)
		return;

	if (playerID < -1)
		playerID = -1;

	void(__thiscall * AddToChatWindowBuffer) (void*, ChatMessageType, const char*, const char*, D3DCOLOR, D3DCOLOR) =
		(void(__thiscall*) (void* _this, ChatMessageType Type, const char* szString, const char* szPrefix, D3DCOLOR TextColor, D3DCOLOR PrefixColor))
		(g_dwSAMP_Addr + SAMP_FUNC_ADDTOCHATWND);

	if (playerID != -1)
	{
		// getPlayerName does the needed validity checks, no need for doubles
		char* playerName = (char*)getPlayerName(playerID);
		if (playerName == NULL)
			return;
		AddToChatWindowBuffer(g_Chat, CHAT_TYPE_CHAT, text, playerName, textColor, samp_color_get(playerID));
	}
	else
	{
		AddToChatWindowBuffer(g_Chat, CHAT_TYPE_DEBUG, text, nullptr, textColor, 0);
	}
}

void restartGame()
{
	if (g_SAMP == NULL)
		return;

	((void(__thiscall*) (void*)) (g_dwSAMP_Addr + SAMP_FUNC_RESTARTGAME)) (g_SAMP);

}

void say(const char* text, ...)
{
	if (g_SAMP == NULL)
		return;

	if (text == NULL)
		return;

	va_list ap;
	char	tmp[128];
	memset(tmp, 0, 128);

	va_start(ap, text);
	vsprintf(tmp, text, ap);
	va_end(ap);

	addSayToChatWindow(tmp);
}

void addSayToChatWindow(char* msg)
{
	if (g_SAMP == NULL)
		return;

	if (msg == NULL)
		return;
	if (isBadPtr_readAny(msg, 128))
		return;
	

	if (msg == NULL)
		return;

	if (msg[0] == '/')
	{
		((void(__thiscall*) (void* _this, char* message)) (g_dwSAMP_Addr + SAMP_FUNC_SENDCMD))(g_Input, msg);
	}
	else
	{
		((void(__thiscall*) (void* _this, char* message)) (g_dwSAMP_Addr + SAMP_FUNC_SAY)) (g_Players->pLocalPlayer, msg);
	}
}

void playerSpawn(void)
{
	if (g_SAMP == NULL)
		return;

	((void(__thiscall*) (void* _this)) (g_dwSAMP_Addr + SAMP_FUNC_REQUEST_SPAWN)) (g_Players->pLocalPlayer);
	((void(__thiscall*) (void* _this)) (g_dwSAMP_Addr + SAMP_FUNC_SPAWN)) (g_Players->pLocalPlayer);
}

void disconnect(int reason /*0=timeout, 500=quit*/)
{
	if (g_SAMP == NULL)
		return;

	g_RakClient->GetInterface()->Disconnect(reason);
}


void sendSetInterior(uint8_t interiorID)
{
	if (g_SAMP == NULL)
		return;

	((void(__thiscall*) (void* _this, byte interiorID)) (g_dwSAMP_Addr + SAMP_FUNC_SENDINTERIOR)) (g_Players->pLocalPlayer, interiorID);
}

void setSpecialAction(uint8_t byteSpecialAction)
{
	if (g_SAMP == NULL)
		return;

	if (g_Players->pLocalPlayer == NULL)
		return;

	((void(__thiscall*) (void* _this, byte specialActionId)) (g_dwSAMP_Addr + SAMP_FUNC_SETSPECIALACTION)) (g_Players->pLocalPlayer, byteSpecialAction);
}

void sendSCMEvent(int iEvent, int iVehicleID, int iParam1, int iParam2)
{
	g_RakClient->SendSCMEvent(iVehicleID, iEvent, iParam1, iParam2);
}

void toggleSAMPCursor(int iToggle)
{
	if (g_SAMP == NULL) return;
	if (g_Input->iInputEnabled) return;

	void* obj = *(void**)(g_dwSAMP_Addr + SAMP_MISC_INFO);
	((void(__thiscall*) (void*, int, bool)) (g_dwSAMP_Addr + SAMP_FUNC_TOGGLECURSOR))(obj, iToggle ? 3 : 0, !iToggle);
	if (!iToggle)
		((void(__thiscall*) (void*)) (g_dwSAMP_Addr + SAMP_FUNC_CURSORUNLOCKACTORCAM))(obj);

}


void sendDeath(void)
{
	if (g_SAMP == NULL)
		return;
	((void(__thiscall*) (void*)) (g_dwSAMP_Addr + SAMP_FUNC_DEATH))
		(g_Players->pLocalPlayer);
}


void updateScoreboardData(void)
{
	((void(__thiscall*) (void* _this)) (g_dwSAMP_Addr + SAMP_FUNC_UPDATESCOREBOARDDATA)) (g_SAMP);
}

void toggleOffScoreboard(void)
{
	((void(__thiscall*) (void* _this, bool hideCursor)) (g_dwSAMP_Addr + SAMP_FUNC_DISABLE_SCOREBOARD)) (g_Scoreboard, true);
}


void __stdcall CNetGame__destructor(void)
{
	// release hooked rakclientinterface, restore original rakclientinterface address and call CNetGame destructor
	if (g_SAMP->pRakClientInterface != NULL)
		delete g_SAMP->pRakClientInterface;
	g_SAMP->pRakClientInterface = g_RakClient->GetInterface();
	return ((void(__thiscall*) (void*)) (g_dwSAMP_Addr + SAMP_FUNC_CNETGAMEDESTRUCTOR))(g_SAMP);
}

void SetupSAMPHook(const char* szName, DWORD dwFuncOffset, void* Func, int iType, int iSize, const char* szCompareBytes)
{
	CDetour api;
	int strl = strlen(szCompareBytes);
	uint8_t* bytes = hex_to_bin(szCompareBytes);

	if (!strl || !bytes || memcmp_safe((uint8_t*)g_dwSAMP_Addr + dwFuncOffset, bytes, strl / 2))
	{
		if (api.Create((uint8_t*)((uint32_t)g_dwSAMP_Addr) + dwFuncOffset, (uint8_t*)Func, iType, iSize) == 0)
			return;
	}
	else
	{
		return;
	}

	if (bytes)
		free(bytes);
}

uint8_t _declspec (naked) hook_handle_rpc_packet(void)
{
	static RPCParameters* pRPCParams = nullptr;
	static RPCNode* pRPCNode = nullptr;
	static DWORD dwTmp = 0;

	__asm pushad;
	__asm mov pRPCParams, eax;
	__asm mov pRPCNode, edi;

	HandleServerRPCPacketFunc(pRPCNode->uniqueIdentifier, pRPCParams, pRPCNode->staticFunctionPointer);
	dwTmp = g_dwSAMP_Addr + SAMP_HOOKEXIT_HANDLE_RPC;

	__asm popad;
	__asm add esp, 4 // overwritten code
	__asm jmp dwTmp;
}

uint8_t _declspec (naked) hook_handle_rpc_packet2(void)
{
	static RPCParameters* pRPCParams = nullptr;
	static RPCNode* pRPCNode = nullptr;
	static DWORD dwTmp = 0;

	__asm pushad;
	__asm mov pRPCParams, ecx;
	__asm mov pRPCNode, edi;

	HandleServerRPCPacketFunc(pRPCNode->uniqueIdentifier, pRPCParams, pRPCNode->staticFunctionPointer);
	dwTmp = g_dwSAMP_Addr + SAMP_HOOKEXIT_HANDLE_RPC2;

	__asm popad;
	__asm jmp dwTmp;
}

void installSAMPHooks()
{
	if (g_SAMP == NULL)
		return;


	SetupSAMPHook("HandleRPCPacket", SAMP_HOOKENTER_HANDLE_RPC, hook_handle_rpc_packet, DETOUR_TYPE_JMP, 6, "FF5701");
	SetupSAMPHook("HandleRPCPacket2", SAMP_HOOKENTER_HANDLE_RPC2, hook_handle_rpc_packet2, DETOUR_TYPE_JMP, 8, "FF5701");
	SetupSAMPHook("CNETGAMEDESTR1", SAMP_HOOKENTER_CNETGAME_DESTR, CNetGame__destructor, DETOUR_TYPE_CALL_FUNC, 5, "E8");
	SetupSAMPHook("CNETGAMEDESTR2", SAMP_HOOKENTER_CNETGAME_DESTR2, CNetGame__destructor, DETOUR_TYPE_CALL_FUNC, 5, "E8");

}

bool OnSendRPC(int uniqueID, BitStream* parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{

	if (netPatchAssoc[uniqueID][OUTCOMING_RPC] != nullptr && netPatchAssoc[uniqueID][OUTCOMING_RPC]->enabled)
		return false;


	return true;
}

bool OnSendPacket(BitStream* parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{

	uint8_t packetId;
	parameters->Read(packetId);
	if (netPatchAssoc[packetId][OUTCOMING_PACKET] != nullptr && netPatchAssoc[packetId][OUTCOMING_PACKET]->enabled)
		return false;


	return true;
}

bool OnReceivePacket(Packet* p)
{
	if (p->data == nullptr || p->length == 0)
		return true;

	if (netPatchAssoc[p->data[0]][INCOMING_PACKET] != nullptr && netPatchAssoc[p->data[0]][INCOMING_PACKET]->enabled)
		return false;


	return true;
}




