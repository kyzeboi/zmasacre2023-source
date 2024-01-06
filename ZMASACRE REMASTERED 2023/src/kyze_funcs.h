/*

	 ** Bu proje kyze. taraf�ndan geli�tirilmektedir. **

*/

#pragma once


bool FuncitonDevoloping()
{
	addMessage(0, "Bu fonksiyon �uan geli�tirilme a�amas�ndad�r.");
	return 1;
}

void LoadSkinModel(int iModelID)
{
	CModelInfo* pModel = pGame->GetModelInfo(iModelID);

	if (pModel->IsLoaded()) return;
	pModel->Request(true);
	while (!pModel->IsLoaded()) Sleep(10);
}

extern void SetSkin(int SkinID);
void SetSkin(int SkinID)
{
	if (SkinID <= -1 || SkinID > 313)
		return addMessage(0, "L�tfen ge�erli bir SkinID giriniz.");
		

	DisembarkInstantly();
	LoadSkinModel(SkinID);
	pPedSelf->SetModelIndex(SkinID);
}

extern void SetHealth(int Health);
void SetHealth(int Health)
{

	if (Health > 100 || Health < -1)
		return addMessage(0, "L�tfen ge�erli bir de�er giriniz. (0-100)");


	struct actor_info* actor_info = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);

	if (actor_info)
	{
		actor_info->hitpoints = Health;
		addMessage(1, "Can de�eriniz de�i�tirildi.");
	}


}

extern void SetArmour(int Armor);
void SetArmour(int Armor)
{

	if (Armor > 100 || Armor < -1)
		return addMessage(0, "L�tfen ge�erli bir de�er giriniz. (0-100)");


	struct actor_info* actor_info = actor_info_get(ACTOR_SELF, ACTOR_ALIVE);

	if (actor_info)
	{
		actor_info->armor = Armor;
		addMessage(1, "Armor de�eriniz de�i�tirildi.");
	}


}

extern void add_friend(int FriendID);
void add_friend(int FriendID)
{

	if (isBadSAMPPlayerID(FriendID) || g_Players->iIsListed[FriendID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	if (g_NewPatch.D�smanDogrulama[FriendID])
	{
		addMessage(0, "Belirtti�iniz oyuncu �u anda d��man tag�nda bulunmaktad�r.");
		addMessage(0, "L�tfen ilk �nce d��man tag�n� kald�r�n.");
	}
	else
	{

		g_NewPatch.FriendID[FriendID] = FriendID;
		g_NewPatch.FriendDogrulama[FriendID] = true;
		g_NewPatch.FriendName[FriendID] = getPlayerName(FriendID);


		addMessage(1, "%s(%d) adl� oyuncu arkada� listene eklendi.", getPlayerName(FriendID), FriendID);


	}

}

extern void delete_friend(int FriendID);
void delete_friend(int FriendID)
{

	if (isBadSAMPPlayerID(FriendID) || g_Players->iIsListed[FriendID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	if (g_NewPatch.D�smanDogrulama[FriendID])
	{
		addMessage(0, "Belirtti�iniz oyuncu �u anda d��man tag�nda bulunmaktad�r.");
		addMessage(0, "L�tfen ilk �nce d��man tag�n� kald�r�n.");
	}
	else
	{

		g_NewPatch.FriendDogrulama[FriendID] = false;
	

		addMessage(1, "%s(%d) adl� oyuncu arkada� listesinden ��kar�ld�.", getPlayerName(FriendID), FriendID);

	}

}

extern void delete_all_friend();
void delete_all_friend()
{
	for (int i = 0; i < 1000; i++)
	{
		if (isBadSAMPPlayerID(i) || g_Players->iIsListed[i] != 1) continue;
		if (g_NewPatch.FriendName[i] != getPlayerName(i)) continue;
	

		g_NewPatch.FriendDogrulama[i] = false;

		addMessage(1, "T�m dostlar�n�z ba�ar�yla silinmi�tir.");
	}
}

extern void add_enemy(int EnemyID);
void add_enemy(int EnemyID)
{

	if (isBadSAMPPlayerID(EnemyID) || g_Players->iIsListed[EnemyID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	if (g_NewPatch.FriendDogrulama[EnemyID])
	{
		addMessage(0, "Belirtti�iniz oyuncu �u anda dost tag�nda bulunmaktad�r.");
		addMessage(0, "L�tfen ilk �nce dost tag�n� kald�r�n.");
	}
	else
	{

		g_NewPatch.D�smanID[EnemyID] = EnemyID;
		g_NewPatch.D�smanDogrulama[EnemyID] = true;
		g_NewPatch.D�smanName[EnemyID] = getPlayerName(EnemyID);


		addMessage(1, "%s(%d) adl� oyuncu d��man listene eklendi.", getPlayerName(EnemyID), EnemyID);


	}

}

extern void delete_enemy(int EnemyID);
void delete_enemy(int EnemyID)
{

	if (isBadSAMPPlayerID(EnemyID) || g_Players->iIsListed[EnemyID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	if (g_NewPatch.FriendDogrulama[EnemyID])
	{
		addMessage(0, "Belirtti�iniz oyuncu �u anda dost tag�nda bulunmaktad�r.");
		addMessage(0, "L�tfen ilk �nce dost tag�n� kald�r�n.");
	}
	else
	{

		g_NewPatch.D�smanDogrulama[EnemyID] = false;


		addMessage(1, "%s(%d) adl� oyuncu d��man listesinden ��kar�ld�.", getPlayerName(EnemyID), EnemyID);

	}

}

extern void delete_all_enemys();
void delete_all_enemys()
{

	for (int i = 0; i < 1000; i++)
	{
		if (isBadSAMPPlayerID(i) || g_Players->iIsListed[i] != 1) continue;
		if (g_NewPatch.D�smanName[i] != getPlayerName(i)) continue;


		g_NewPatch.D�smanDogrulama[i] = false;

		addMessage(1, "T�m d��manlar�n�z ba�ar�yla silinmi�tir.");

	}

}

extern void warp(int PlayerID);
void warp(int PlayerID)
{

	if (isBadSAMPPlayerID(PlayerID) || g_Players->iIsListed[PlayerID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	float pos[3];
	actor_info* actor;
	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[PlayerID]))
			return addMessage(0, "Oyuncu bulunamad�.");

		vect3_copy(g_stStreamedOutInfo.fPlayerPos[PlayerID], pos);
		pos[1] += 1.0f;
		cheat_teleport(pos, 0);
		return;
	}

	if (!getPlayerPos(PlayerID, pos) ||
		g_Players->pRemotePlayer[PlayerID]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		return addMessage(0, "Oyuncu bulunamad�.");
	}

	pos[1] += 1.0f;
	cheat_teleport(pos, actor->base.interior_id);
	addMessage(1, "Ba�ar�yla belirtti�iniz konuma ���nland�n�z.");

}

extern void spec(int PlayerID);
void spec(int PlayerID)
{

	if (isBadSAMPPlayerID(PlayerID) || g_Players->iIsListed[PlayerID] != 1)
		return addMessage(0, "Belirtti�iniz oyuncu oyunda de�il.");

	if (g_Players->pRemotePlayer[PlayerID]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor == nullptr)
		return addMessage(0, "Oyuncu bulunamad�");


	spectatePlayer(PlayerID);

	addMessage(1, "%s(%d) adl� oyuncuyu izliyorsunuz.", getPlayerName(PlayerID), PlayerID);
	addMessage(2, "E�er izlemi i�lemini bitirmek istiyorsan�z l�tfen End Spectate tu�una bas�n�z.");


}

extern void end_spec();
void end_spec()
{
	if (g_iSpectateEnabled != 1)
		return addMessage(0, "Kimseyi izlemiyorsunuz.");

	spectatePlayeroff(g_iSpectatePlayerID);

	addMessage(2, "Spectate i�lemi durduruldu.");

}

extern void Set_Car_Color(int CarID);
void Set_Car_Color(int CarID)
{
	struct vehicle_info* info = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);


	if (!IsPlayerInVehicle())
		return addMessage(0, "Bu hileyi kullanabilmek i�in ara�ta olmal�s�n�z.");
	
	if (info)
	{
		if (car_color == 0)
		{
			vehicle_setColor0(info, 003);
			vehicle_setColor1(info, 003);
		}
		else if (car_color == 1)
		{
			vehicle_setColor0(info, 128);
			vehicle_setColor1(info, 128);
		}
		else if (car_color == 2)
		{
			vehicle_setColor0(info, 000);
			vehicle_setColor1(info, 000);
		}
		else if (car_color == 3)
		{
			vehicle_setColor0(info, 162);
			vehicle_setColor1(info, 162);
		}
		else if (car_color == 4)
		{
			vehicle_setColor0(info, 134);
			vehicle_setColor1(info, 134);
		}
		
		addMessage(1, "Ara� rengi de�i�tirildi.");
	}

	

}

void SetHeath(vehicle_info* vehicle, int Health)
{
	
	CVehicle* pVehicle = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);
	if (pVehicle)
		pVehicle->SetHealth(Health);
}


extern void Set_Car_Health (int CarHealth);
void Set_Car_Health(int CarHealth)
{
	struct vehicle_info* info = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);

	if (!IsPlayerInVehicle())
		return addMessage(0, "Bu hileyi kullanabilmek i�in ara�ta olmal�s�n�z.");

	if (info)
	{
		SetHeath(info, CarHealth);
		addMessage(1, "Ara� can� de�i�tirildi.");
	}
}


void FixWiew(vehicle_info* vehicle)
{
	CVehicle* pVehicle = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);
	if (pVehicle)
		pVehicle->Fix();
	
}

extern void Reset_Car_Wiew();
void Reset_Car_Wiew()
{
	struct vehicle_info* info = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);

	if (!IsPlayerInVehicle)
		return addMessage(0, "Bu hileyi kullanabilmek i�in ara�ta olmal�s�n�z.");

	if (info)
	{
		FixWiew(info);
		addMessage(1, "Ara� g�r�n�m� s�f�rland�.");
	}
}


extern void Set_Random_Car_Color();
void Set_Random_Car_Color()
{
	struct vehicle_info* info = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);

	if (!IsPlayerInVehicle())
		return addMessage(0, "Bu hileyi kullanabilmek i�in ara�ta olmal�s�n�z.");

	if (info)
	{
		if (random_car_color_id == 0)
		{
			int rand_color = rand() % 255;
			vehicle_setColor0(info, rand_color);
			vehicle_setColor1(info, rand_color);
		}
		else
		{
			vehicle_setColor0(info, rand() % 255);
			vehicle_setColor1(info, rand() % 255);
		}

		addMessage(1, "Ara� rengi random �ekilde ayarland�.");
	}
}