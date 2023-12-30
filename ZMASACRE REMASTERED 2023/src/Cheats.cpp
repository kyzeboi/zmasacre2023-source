/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#include "main.h"


struct playerTagInfo
{
#pragma pack( 1 )
	CVector tagPosition;
	float	tagOffsetY;
	bool	isStairStacked;
	float	stairStackedOffset;
	bool	isPastMaxDistance;
} g_playerTagInfo[SAMP_MAX_PLAYERS];

bool playershowed;

void renderPlayerT4gs(void)
{
	// don't run in the menu
	if (gta_menu_active())
		return;



	if (render_player_t4gs)
		sampPatchDisableNameTags(1);
	else
		sampPatchDisableNameTags(0);

	if (!render_player_t4gs)
		return;

	if (ÝmMenu->show)
		return;


	// don't run during certain samp events
	if (g_dwSAMP_Addr && g_SAMP)
	{

		if (
			// Scoreboard open?
			(GetAsyncKeyState(VK_TAB) < 0)
			|| g_Scoreboard->iIsEnabled
			// F10 key down?
			|| GetAsyncKeyState(VK_F10) < 0
			)
		{
			return;
		}
	}

	// don't run if the CGameSA doesn't exist
	if (!pGameInterface)
		return;

	// don't run if we don't exist
	if (isBadPtr_GTA_pPed(pPedSelf))
		return;

	// for tracking player states as we iterate through
	bool	isPedESPCollided[SAMP_MAX_PLAYERS];
	bool	isPedESPStairStacked[SAMP_MAX_PLAYERS];
	memset(isPedESPCollided, false, sizeof(bool) * SAMP_MAX_PLAYERS);
	memset(isPedESPStairStacked, true, sizeof(bool) * SAMP_MAX_PLAYERS);

	// alignment settings
	int			ESP_tag_player_pixelOffsetY = -10;
	float		ESP_tag_player_D3DBox_pixelOffsetX = -0.5f;
	float		ESP_tag_player_D3DBox_pixelOffsetY = -0.5f;
	float		ESP_tag_player_posOffsetZ = 1.0f;
	float		ESP_tag_player_espHeight = 20.0f;
	//float		ESP_tag_player_movementSpeed = 5.0f;

	// trash values to use during iterations
	float		vh, va, ping;
	int			iGTAID, iGTAID_Inner, selfGTAID;
	CVector		iterPosition, ourPosMinusIter, ourPosition;
	D3DXVECTOR3 poss, screenposs;
	char		buf[256];
	char		buf2[256];
	char		buf3[256];

	// get our info
	if (pPedSelf->GetVehicle())
	{
		// RC Vehicle Fix (Not showing names of recently streamed in players
		ourPosition = *(pPedSelf->GetVehicle()->GetPosition());
		// while being in a RC Vehicle)
	}
	else
	{
		ourPosition = pPedSelfSA->Placeable.matrix->vPos;
	}

	// get our scripting ID so we can exclude ourself
	selfGTAID = (int)pPedSelf->GetArrayID();

	// setup iterator
	CPedSA* iterPed = NULL;
	CPoolsSA* pPools = reinterpret_cast <CPoolsSA*> (pGameInterface->GetPools());
	CPoolsSA::pedPool_t::mapType::iterator iter = pPools->m_pedPool.map.begin();
	// get initial variables for peds streamed in
	while (iter.pos < iter.end)
	{
		// map iterator pointer to our pointer
		iterPed = iter.pos->second;

		// advance to next item for next pass
		iter.pos++;
		if (!iterPed)
			continue;

		// get player id
		iGTAID = (int)iterPed->GetArrayID();

		// ignore if it's us
		if (iGTAID == selfGTAID)
			continue;

		// RC Vehicle fix (not showing names of players in RC vehicles)
		if (iterPed->GetVehicle() != NULL)
		{
			if (gta_vehicle_get_by_id(iterPed->GetVehicle()->GetModelIndex())->class_id == VEHICLE_CLASS_MINI
				&& iterPed->GetVehicle()->GetDriver() == iterPed)
			{
				iterPosition = *iterPed->GetVehicle()->GetPosition();
			}
			else
			{
				iterPosition = iterPed->GetInterface()->Placeable.matrix->vPos;
			}
		}
		else
		{
			iterPosition = iterPed->GetInterface()->Placeable.matrix->vPos;
		}

		// check if it's farther than set.player_tags_dist
		ourPosMinusIter = ourPosition - iterPosition;
		if (ourPosMinusIter.Length() > 1000)
		{
			g_playerTagInfo[iGTAID].isPastMaxDistance = true;
			continue;
		}
		else
			g_playerTagInfo[iGTAID].isPastMaxDistance = false;



		// get the player position in 2D
		poss.x = iterPosition.fX;
		poss.y = iterPosition.fY;
		poss.z = iterPosition.fZ + ESP_tag_player_posOffsetZ;
		CalcScreenCoors(&poss, &screenposs);

		// check if the iter is culled or not
		if (screenposs.z < 1.f)
		{
			g_playerTagInfo[iGTAID].tagOffsetY = 0.0f;
			g_playerTagInfo[iGTAID].isPastMaxDistance = true;
			continue;
		}

		// global, set ESP position for tagOffsetY use
		g_playerTagInfo[iGTAID].tagPosition.fX = screenposs.x;
		g_playerTagInfo[iGTAID].tagPosition.fY = screenposs.y;
		g_playerTagInfo[iGTAID].tagPosition.fZ = screenposs.z;
	}
	// reset iter position & setup iterInner
	iter = pPools->m_pedPool.map.begin();

	CPedSA* iterInnerPed = NULL;
	CPoolsSA::pedPool_t::mapType::iterator iterInner;

	// remove staircase problem
	while (iter.pos < iter.end)
	{
		// map iterator pointer to our pointer
		iterPed = iter.pos->second;

		// advance to next item for next pass
		iter.pos++;
		if (!iterPed)
			continue;

		// get player id
		iGTAID = (int)iterPed->GetArrayID();

		// ignore if it's us
		if (iGTAID == selfGTAID)
			continue;

		// filter out "ok" ESP
		if (g_playerTagInfo[iGTAID].isPastMaxDistance
			|| !g_playerTagInfo[iGTAID].isStairStacked
			&& g_playerTagInfo[iGTAID].tagOffsetY < 40.0f
			)
			continue;

		// detect stair stacking per frame if ESP isn't already stair stacked
		if (!g_playerTagInfo[iGTAID].isStairStacked)
		{
			// reset iterInner position
			iterInner = pPools->m_pedPool.map.begin();
			while (iterInner.pos < iterInner.end)
			{
				// map iterator pointer to our pointer
				iterInnerPed = iterInner.pos->second;

				// advance to next item for next pass
				iterInner.pos++;
				if (!iterInnerPed)
					continue;

				// get player id
				iGTAID_Inner = (int)iterInnerPed->GetArrayID();

				// ignore if it's us or isPastMaxDistance
				if (g_playerTagInfo[iGTAID_Inner].isPastMaxDistance || iGTAID_Inner == iGTAID)
					continue;

				// test to see who comes out on top
				if (abs(g_playerTagInfo[iGTAID].tagPosition.fX - g_playerTagInfo[iGTAID_Inner].tagPosition.fX) <= 100.0f
					&& abs((g_playerTagInfo[iGTAID].tagPosition.fY - (g_playerTagInfo[iGTAID].tagOffsetY / 2.0f)) - (g_playerTagInfo[iGTAID_Inner].tagPosition.fY - g_playerTagInfo[iGTAID_Inner].tagOffsetY)) <= ESP_tag_player_espHeight)
				{
					isPedESPStairStacked[iGTAID] = false;
				}
			}

			// setup stair stack variables needed to un stack the ESP
			if (isPedESPStairStacked[iGTAID])
			{
				g_playerTagInfo[iGTAID].isStairStacked = true;
				g_playerTagInfo[iGTAID].stairStackedOffset = g_playerTagInfo[iGTAID].tagOffsetY / 2.0f;
			}
		}	// end inner while - detect stair stacking

		// lower the offsets for stair stacked ESP
		// and turn off stack status of ESP that reaches the "available" offset
		if (g_playerTagInfo[iGTAID].isStairStacked)
		{
			g_playerTagInfo[iGTAID].tagOffsetY -= 5.0f;
			g_playerTagInfo[iGTAID].stairStackedOffset -= 5.0f;
			if (g_playerTagInfo[iGTAID].stairStackedOffset < 5.0f)
			{
				g_playerTagInfo[iGTAID].stairStackedOffset = 0.0f;
				g_playerTagInfo[iGTAID].isStairStacked = false;
			}
		}
	}		// end outer while - remove staircase problem

	// reset iter position & setup iterInner
	iter = pPools->m_pedPool.map.begin();

	// detect & adjust for ESP collisions
	while (iter.pos < iter.end)
	{
		// map iterator pointer to our pointer
		iterPed = iter.pos->second;

		// advance to next item for next pass
		iter.pos++;
		if (!iterPed)
			continue;

		// get player id
		iGTAID = (int)iterPed->GetArrayID();

		// we isPastMaxDistance or stairstacked, move along
		if (g_playerTagInfo[iGTAID].isPastMaxDistance || g_playerTagInfo[iGTAID].isStairStacked)
			continue;

		// reset iterInner position
		iterInner = pPools->m_pedPool.map.begin();
		while (iterInner.pos < iterInner.end)
		{
			// map iterator pointer to our pointer
			iterInnerPed = iterInner.pos->second;

			// advance to next item for next pass
			iterInner.pos++;
			if (!iterInnerPed)
				continue;

			// get player id
			iGTAID_Inner = (int)iterInnerPed->GetArrayID();

			// filter out isPastMaxDistance, stairstacked, and same Ped
			if (g_playerTagInfo[iGTAID].isPastMaxDistance
				|| g_playerTagInfo[iGTAID_Inner].isStairStacked
				|| iGTAID == iGTAID_Inner) continue;

			// player is within range, figure out if there's collision
			if (abs(g_playerTagInfo[iGTAID].tagPosition.fX - g_playerTagInfo[iGTAID_Inner].tagPosition.fX) <= 100.0f
				&& abs((g_playerTagInfo[iGTAID].tagPosition.fY - g_playerTagInfo[iGTAID].tagOffsetY) - (
					g_playerTagInfo[iGTAID_Inner].tagPosition.fY - g_playerTagInfo[iGTAID_Inner].tagOffsetY)) <= ESP_tag_player_espHeight)
			{
				// collision, figure out who gets to stay
				if (g_playerTagInfo[iGTAID].tagPosition.fZ < g_playerTagInfo[iGTAID_Inner].tagPosition.fZ)
				{
					// playerID "g_pTI_i" is farther, it should move up
					g_playerTagInfo[iGTAID_Inner].tagOffsetY += 5.0f;
					isPedESPCollided[iGTAID_Inner] = true;
				}
				else if (g_playerTagInfo[iGTAID].tagPosition.fZ > g_playerTagInfo[iGTAID_Inner].tagPosition.fZ)
				{
					// playerID "i" is farther, it should move up
					// we should only need normal upward movement here
					g_playerTagInfo[iGTAID].tagOffsetY += 5.0f;
					isPedESPCollided[iGTAID] = true;
				}
				else
				{
					// both playerIDs are the same position @_@ so prefer the lower ID#
					if (iGTAID < iGTAID_Inner)
					{
						g_playerTagInfo[iGTAID_Inner].tagOffsetY += 5.0f;
						isPedESPCollided[iGTAID_Inner] = true;
					}
					else
					{
						g_playerTagInfo[iGTAID].tagOffsetY += 5.0f;
						isPedESPCollided[iGTAID] = true;
					}
				}
			}

			// are we jigglin?  everybody likes ta jiggle.
			if (abs(g_playerTagInfo[iGTAID].tagPosition.fX - g_playerTagInfo[iGTAID_Inner].tagPosition.fX) <= 100.0f
				&& abs(
					(g_playerTagInfo[iGTAID].tagPosition.fY - g_playerTagInfo[iGTAID].tagOffsetY)
					- (g_playerTagInfo[iGTAID_Inner].tagPosition.fY - g_playerTagInfo[iGTAID_Inner].tagOffsetY)
				) - 5.0f <= ESP_tag_player_espHeight
				)
			{
				if (g_playerTagInfo[iGTAID].tagPosition.fZ < g_playerTagInfo[iGTAID_Inner].tagPosition.fZ)
				{
					isPedESPCollided[iGTAID_Inner] = true;
				}
				else
				{
					isPedESPCollided[iGTAID] = true;
				}
			}
		}	// end inner while

		// return tagOffsetY to zero if needed
		if (!isPedESPCollided[iGTAID])
		{
			if (g_playerTagInfo[iGTAID].tagOffsetY >= 5.0f)
			{
				g_playerTagInfo[iGTAID].tagOffsetY -= 5.0f;
			}
			else
			{
				g_playerTagInfo[iGTAID].tagOffsetY = 0.0f;
			}
		}
	}		// end outer while

	// reset iter position & setup iterInner
	iter = pPools->m_pedPool.map.begin();

	// start render ESP tags
	float h, playerBaseY;
	while (iter.pos < iter.end)
	{
		// map iterator pointer to our pointer
		iterPed = iter.pos->second;

		// advance to next item for next pass
		iter.pos++;
		if (!iterPed)
			continue;

		// get player id
		iGTAID = (int)iterPed->GetArrayID();

		// ignore if isPastMaxDistance or if it's us
		if (g_playerTagInfo[iGTAID].isPastMaxDistance || iGTAID == selfGTAID)
			continue;

		playerBaseY = g_playerTagInfo[iGTAID].tagPosition.fY -
			g_playerTagInfo[iGTAID].tagOffsetY +
			ESP_tag_player_pixelOffsetY;

		int iSAMPID;
		if (g_Players)
			iSAMPID = translateGTASAMP_pedPool.iSAMPID[getPedGTAIDFromInterface((DWORD*)iterPed->GetPedInterface())];

		// get Ped health
		// works in single player, but SAMP maintains its own player health
		//vh = iterPed->GetHealth();
		// get samp health
		stRemotePlayer* pPlayerR = g_SAMP->pPools->pPlayer->pRemotePlayer[iSAMPID];
		if (g_Players)
		{

			if (g_Players->pRemotePlayer[iSAMPID] != NULL
				&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData != NULL
				&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor != NULL
				&& (DWORD)g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor->pGTA_Ped == (DWORD)iterPed->GetPedInterface())
			{
				vh = g_Players->pRemotePlayer[iSAMPID]->pPlayerData->fActorHealth;
				va = g_Players->pRemotePlayer[iSAMPID]->pPlayerData->fActorArmor;
				ping = pPlayerR->iPing;
			}
			else
			{
				// SA-MP running, but was not a remote player
				continue;
			}
		}
		else
		{
			// SA-MP not running or failed to initialize g_Players
			vh = iterPed->GetHealth();
			va = iterPed->GetArmor();
			ping = pPlayerR->iPing;
		}



		g_playerTagInfo[iGTAID].tagPosition.fX -= 50.0f;
		updateScoreboardData();

		D3DCOLOR    color;

		color = D3DCOLOR_ARGB(160, 70, 170, 255);
		if (vh > 100.0f)
			vh = 100.0f;
		if (vh < 100.0f && vh > 90.0f)
			color = D3DCOLOR_ARGB(160, 70, 170, 255);
		if (vh < 90.0f && vh > 60.0f)
			color = D3DCOLOR_ARGB(160, 106, 90, 255);
		if (vh < 60.0f && vh > 40.0f)
			color = D3DCOLOR_ARGB(160, 199, 21, 133);
		if (vh < 40.0f && vh > 20.0f)
			color = D3DCOLOR_ARGB(70, 255, 0, 255);
		if (vh < 20.0f && vh > 0.0f)
			color = D3DCOLOR_ARGB(160, 102, 0, 0);
			
		



		render->D3DBox(g_playerTagInfo[iGTAID].tagPosition.fX + ESP_tag_player_D3DBox_pixelOffsetX,
			playerBaseY + ESP_tag_player_D3DBox_pixelOffsetY, 100.0f, 21.0f, D3DCOLOR_ARGB(70, 0, 0, 0));
		render->D3DBox(g_playerTagInfo[iGTAID].tagPosition.fX + 1.0f + ESP_tag_player_D3DBox_pixelOffsetX,
			playerBaseY + 1.0f + ESP_tag_player_D3DBox_pixelOffsetY, vh - 2.0f, 10.0f, color);



		float offY = ESP_tag_player_D3DBox_pixelOffsetY;
		float armbar = va;
		if (armbar > 100.0f)
			armbar = 100.0f;
		render->D3DBox(g_playerTagInfo[iGTAID].tagPosition.fX + 1.0f + ESP_tag_player_D3DBox_pixelOffsetX,
			playerBaseY + 1.0f + ESP_tag_player_D3DBox_pixelOffsetY + 11.f, va - 2.0f, 9.0f, D3DCOLOR_ARGB(160, 186, 186, 186));
		offY += 10.0f;


		// this should also calculate the anti-aliasing top edge somehow
		h = pD3DFontFixedSmall->DrawHeight() + 1;


		_snprintf_s(buf, sizeof(buf) - 1, "       < %d >", (int)vh);
		pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 8.0f, playerBaseY - h + 10.0f,
			D3DCOLOR_ARGB(255, 255, 125, 000), buf);

		_snprintf_s(buf, sizeof(buf) - 1, "       < %d >", (int)va);
		pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 8.0f, playerBaseY - h + 23.0f,
			D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), buf);

		float fPlayerNameTagX = g_playerTagInfo[iGTAID].tagPosition.fX + ESP_tag_player_D3DBox_pixelOffsetX / 2 + 50.0f - pD3DFontFixedSmall->DrawLength(buf) / 2;


		switch (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->bytePlayerState)
		{
		case PLAYER_STATE_ONFOOT:
		{
			pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
				D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), "Onfoot");
			break;
		}
		case PLAYER_STATE_DRIVER:
			pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
				D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), "Driver");
			break;
		case PLAYER_STATE_PASSENGER:
			if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->iPassengerDriveBy)
			{
				pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
					D3DCOLOR_ARGB(255, 255, 255, 255), "Drive-By");
			}
			else
			{
				pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
					D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), "Passenger");
				break;
			}
		case PLAYER_STATE_SPAWNED:

			pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
				D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), "Spawned");
			break;

		case PLAYER_STATE_WASTED:

			pD3DFontFixedSmall->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX + 60.0f, playerBaseY - h + 36.0f,
				D3DCOLOR_ARGB(135, 0xFF, 0x9F, 0), "Dead");
			break;

		}


		char        bufdwa[256];
		for (int i = 0; i < 45; i++)
		{

			if (getPlayerState(iSAMPID) == PLAYER_STATE_NONE || PLAYER_STATE_ONFOOT || PLAYER_STATE_SPAWNED || PLAYER_STATE_WASTED)
			{
				if (weapon_list[i].id == g_Players->pRemotePlayer[iSAMPID]->pPlayerData->onFootData.byteCurrentWeapon)
				{
					if (weapon_list[i].id != 0)
					{
						if (weapon_list[i].id == 30) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "H");
						else if (weapon_list[i].id == 31) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "5");
						else if (weapon_list[i].id == 24) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "3");
						else if (weapon_list[i].id == 23) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "2");
						else if (weapon_list[i].id == 22) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "6");
						else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "=");
						else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "9");
						else if (weapon_list[i].id == 29) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "8");
						else if (weapon_list[i].id == 32) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "7");
						else if (weapon_list[i].id == 33) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, ".");
						else if (weapon_list[i].id == 34) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "A");
						else if (weapon_list[i].id == 38) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "F");
						else _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "");
						float w = pD3DFontWeapon->DrawLength(bufdwa);
						h = pD3DFontWeapon->DrawHeight() - 9;
						pD3DFontWeapon->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX - 35.0f, playerBaseY + offY - 20.0f,
							D3DCOLOR_XRGB(205, 37, 118), bufdwa);
					}
					break;
				}
			}
			if (getPlayerState(iSAMPID) == PLAYER_STATE_PASSENGER)
			{
				if (weapon_list[i].id == g_Players->pRemotePlayer[iSAMPID]->pPlayerData->passengerData.byteCurrentWeapon)
				{
					if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->iPassengerDriveBy)
					{
						if (weapon_list[i].id != 0)
						{
							if (weapon_list[i].id == 30) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "H");
							else if (weapon_list[i].id == 31) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "5");
							else if (weapon_list[i].id == 24) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "3");
							else if (weapon_list[i].id == 23) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "2");
							else if (weapon_list[i].id == 22) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "6");
							else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "=");
							else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "9");
							else if (weapon_list[i].id == 29) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "8");
							else if (weapon_list[i].id == 32) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "7");
							else if (weapon_list[i].id == 33) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, ".");
							else if (weapon_list[i].id == 34) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "A");
							else if (weapon_list[i].id == 38) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "F");
							else _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "");
							pD3DFontWeapon->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX - 35.0f, playerBaseY + offY - 20.0f,
								D3DCOLOR_XRGB(211, 26, 26), bufdwa);
						}

						break;
					}
					else {
						if (weapon_list[i].id != 0)
						{
							if (weapon_list[i].id == 30) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "H");
							else if (weapon_list[i].id == 31) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "5");
							else if (weapon_list[i].id == 24) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "3");
							else if (weapon_list[i].id == 23) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "2");
							else if (weapon_list[i].id == 22) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "6");
							else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "=");
							else if (weapon_list[i].id == 25) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "9");
							else if (weapon_list[i].id == 29) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "8");
							else if (weapon_list[i].id == 32) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "7");
							else if (weapon_list[i].id == 33) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, ".");
							else if (weapon_list[i].id == 34) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "A");
							else if (weapon_list[i].id == 38) _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "F");
							else _snprintf_s(bufdwa, sizeof(bufdwa) - 1, "");
							float w = pD3DFontWeapon->DrawLength(bufdwa);
							h = pD3DFontWeapon->DrawHeight() - 9;
							pD3DFontWeapon->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX - 35.0f, playerBaseY + offY - 20.0f,
								D3DCOLOR_XRGB(205, 37, 118), bufdwa);
						}
						break;

					}

				}
			}
		}


		if (g_NewPatch.FriendDogrulama[iSAMPID] == false && g_NewPatch.DüsmanDogrulama[iSAMPID] == false)
		{


			h = pD3DFont->DrawHeight() - 1;
			sprintf_s(buf, "{ededed}%s - ID: %d - Level: %d", getPlayerName(iSAMPID), iSAMPID, g_Players->pRemotePlayer[iSAMPID]->iScore);
			pD3DFont->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX, playerBaseY - h - 8.0f, D3DCOLOR_ARGB(255, 255, 255, 255), buf);
		}




		if (g_NewPatch.FriendID[iSAMPID] && g_NewPatch.FriendDogrulama[iSAMPID] && g_NewPatch.FriendName[iSAMPID] == getPlayerName(iSAMPID))
		{


			h = pD3DFont->DrawHeight() - 1;
			sprintf_s(buf, "%s - ID: %d - Level: %d", getPlayerName(iSAMPID), iSAMPID, g_Players->pRemotePlayer[iSAMPID]->iScore);
			pD3DFont->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX, playerBaseY - h - 8.0f, /*D3DCOLOR_ARGB(255, 31, 206, 31)*/D3DCOLOR_ARGB(255, 000, 255, 000), buf);

		}
		else if (g_NewPatch.DüsmanID[iSAMPID] && g_NewPatch.DüsmanDogrulama[iSAMPID] && g_NewPatch.DüsmanName[iSAMPID] == getPlayerName(iSAMPID))
		{
			h = pD3DFont->DrawHeight() - 1;
			sprintf_s(buf, "%s - ID: %d - Level: %d", getPlayerName(iSAMPID), iSAMPID, g_Players->pRemotePlayer[iSAMPID]->iScore);
			pD3DFont->PrintShadow(g_playerTagInfo[iGTAID].tagPosition.fX, playerBaseY - h - 8.0f, KIRMIZII, buf);

		}


		if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->iAFKState == 2)
		{
			_snprintf_s(buf, sizeof(buf) - 1, "AFK");

			float w = pD3DFontFixedSmall->DrawLength(buf);
			h = pD3DFontFixedSmall->DrawHeight() + 1;

			fPlayerNameTagX = g_playerTagInfo[iGTAID].tagPosition.fX + ESP_tag_player_D3DBox_pixelOffsetX / 2 + 50.0f - pD3DFont->DrawLength(buf) / 2;

			float fAFKTextY = playerBaseY - h + 10.0f + 8.0f + 2.0f + 3.0f;

			if (getPlayerState(iSAMPID) == PLAYER_STATE_DRIVER)
				fAFKTextY += 1.0f;

			render->D3DBox(fPlayerNameTagX + 62.0f, fAFKTextY - 10.0f, pD3DFont->DrawLength(buf) + 2.0f, 10.0f, D3DCOLOR_ARGB(111, 13, 13, 13));
			pD3DFontFixedSmall->PrintShadow(fPlayerNameTagX + 63.0f, fAFKTextY - 12.0f, D3DCOLOR_ARGB(255, 232, 0, 0), buf);
		}



	}


}


void RenderSpecialFuncs()
{

	if (!gta_menu_active())
	{

		if (NoReload)
		{
			struct actor_info* pInfo = actor_info_get(ACTOR_SELF, 0);
			if (pInfo->weapon[pInfo->weapon_slot].ammo_clip > 0)
			{

				pInfo->weapon[pInfo->weapon_slot].ammo_clip++;
				if (pInfo->weapon[pInfo->weapon_slot].ammo_clip = -1)
				{
					pInfo->weapon[pInfo->weapon_slot].ammo_clip--;
				}

			}
		}

		if (autoaim) //geliþtirilecek
		{


		}

		if (box)
		{
			actor_info* player = getGTAPedFromSAMPPlayerID(aimedplayer);
			if (!player)
				return;
			CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);
			if (!pPed)
				return;

			float
				max_up_val = 0,
				max_down_val = 0,
				max_left_val = 0,
				max_right_val = 0;

			CVector mySpinePos, TargetSpinePos;

			for (int bone_id = BONE_PELVIS1; bone_id <= BONE_RIGHTFOOT; ++bone_id)
			{
				if (bone_id >= BONE_PELVIS1 && bone_id <= BONE_HEAD ||
					bone_id >= BONE_RIGHTUPPERTORSO && bone_id <= BONE_RIGHTTHUMB ||
					bone_id >= BONE_LEFTUPPERTORSO && bone_id <= BONE_LEFTTHUMB ||
					bone_id >= BONE_LEFTHIP && bone_id <= BONE_LEFTFOOT ||
					bone_id >= BONE_RIGHTHIP && bone_id <= BONE_RIGHTFOOT)
				{

					CVector bone;
					pPed->GetBonePosition((eBone)bone_id, &bone);


					pPed->GetBonePosition(BONE_SPINE1, &TargetSpinePos);


					D3DXVECTOR3 bone_pos;

					bone_pos.x = bone.fX;
					bone_pos.y = bone.fY;
					bone_pos.z = bone.fZ;
					D3DXVECTOR3 bone_screen_pos;
					CalcScreenCoors(&bone_pos, &bone_screen_pos);

					// check if the iter is culled or not
					if (bone_screen_pos.z < 1.f)
						break;



					if (!max_up_val && !max_down_val &&
						!max_left_val && !max_right_val)
					{
						max_up_val = bone_screen_pos.y;
						max_down_val = bone_screen_pos.y;
						max_right_val = bone_screen_pos.x;
						max_left_val = bone_screen_pos.x;
					}


					if (bone_screen_pos.y > max_up_val)
						max_up_val = bone_screen_pos.y;

					if (bone_screen_pos.y < max_down_val)
						max_down_val = bone_screen_pos.y;

					if (bone_screen_pos.x > max_right_val)
						max_right_val = bone_screen_pos.x;

					if (bone_screen_pos.x < max_left_val)
						max_left_val = bone_screen_pos.x;

				}
			}

			float height_box = max_up_val - max_down_val;
			float widht_box = max_right_val - max_left_val + 12.0f;

			D3DCOLOR color = KIRMIZII;

			render->D3DBoxBorder(max_left_val,
				max_down_val,
				widht_box,
				height_box, color, 255);

		}

#define TYPE_ONE 0
#define TYPE_MULTI 1

		if (trace)
		{
			for (int playerid = 0; playerid < SAMP_MAX_PLAYERS; playerid++)
			{
				actor_info* player;
				switch (trace_type)
				{
				case TYPE_ONE:
					player = getGTAPedFromSAMPPlayerID(aimedplayer);
					break;
				case TYPE_MULTI:
					player = getGTAPedFromSAMPPlayerID(playerid);
					break;
				default:
					break;
				}

				actor_info* me = actor_info_get(ACTOR_SELF, NULL);

				if (!player)
					continue;

				if (me == player)
					continue;

				CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)player);

				if (!pPed)
					return;

				CVector MyHead, TargetSpinePos;

				pPedSelf->GetBonePosition(BONE_SPINE1, &MyHead);
				pPed->GetBonePosition(BONE_HEAD, &TargetSpinePos);
				D3DCOLOR color = KIRMIZII;


				if (render)
					render->DrawLine(CVecToD3DXVEC(MyHead), CVecToD3DXVEC(TargetSpinePos), color);
			}


		}

		if (ShowEnemyPanel)
		{
			float ypos = pPresentParam.BackBufferHeight + 515;
			render->D3DBox(pPresentParam.BackBufferWidth + 980 * 2  - 280, pPresentParam.BackBufferHeight + 500, 250.0f, 250.0f, D3DCOLOR_ARGB(111, 0, 0, 0));

			int count = 0;
			
			for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
			{
				if (g_Players->iIsListed[i] != 1)
					continue;
				if (g_Players->pRemotePlayer[i] == NULL)
					continue;

				for (int x = 0; x < 1; x++)
				{
					if (getPlayerName(i) == NULL)
						break;

					if (g_NewPatch.DüsmanName[i] == "") break;

					D3DCOLOR color;
					char buffer[64];
					ZeroMemory(buffer, sizeof(buffer));

					if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[i]) && (g_Players->pRemotePlayer[i]->pPlayerData == NULL || g_Players->pRemotePlayer[i]->pPlayerData->pSAMP_Actor == NULL))
					{
						color = D3DCOLOR_XRGB(255, 255, 255);
						sprintf(buffer, "%s[%d] - No In Rage", getPlayerName(i), i);
					}
					else
					{
						color = D3DCOLOR_XRGB(30, 144, 255);

						char		weaponname[256];
						for (int weapon_id = 0; weapon_id < 45; weapon_id++)
						{
							if (weapon_list[weapon_id].id == g_Players->pRemotePlayer[i]->pPlayerData->onFootData.byteCurrentWeapon ||
								weapon_list[weapon_id].id == g_Players->pRemotePlayer[i]->pPlayerData->passengerData.byteCurrentWeapon)
							{
								{
									sprintf(weaponname, "[%s]", weapon_list[weapon_id].name);
								}
								
							}
						}

						sprintf(buffer, "%s[%d] - In range {D31A1A} %s", getPlayerName(i), i, weaponname);
						
					}

					pD3DFontFixedSmall->PrintShadow(pPresentParam.BackBufferWidth + 980 * 2 - 280, ypos, color, buffer);
					ypos += pD3DFontFixedSmall->DrawHeight();
					count++;
					updateScoreboardData();
				}
			}




			char count_buffer[32];
			ZeroMemory(count_buffer, sizeof(count_buffer));

			sprintf(count_buffer, "Online Enemys: [%d]", count);
			pD3DFontFixedSmall->PrintShadow(pPresentParam.BackBufferWidth + 980 * 2 - 280, pPresentParam.BackBufferHeight + 500, D3DCOLOR_XRGB(255, 165, 0), count_buffer);

			
			
		}





	}


}