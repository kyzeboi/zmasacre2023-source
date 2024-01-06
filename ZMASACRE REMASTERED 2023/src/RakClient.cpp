/*

	 ** Bu proje kyze. tarafından geliştirilmektedir. **

*/


#include "main.h"


BYTE GetPacketID(Packet* p)
{
	if (p == 0) return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char)p->data[0];
}

bool HookedRakClientInterface::RPC(int* uniqueID, BitStream* parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (uniqueID != nullptr)
	{
		

		if (*uniqueID == RPC_DialogResponse)
		{


			short int wDialogID;
			byte bResponse;
			short int wListItem;
			byte bTextLength;
			char Text[30];

			parameters->Read(wDialogID);
			parameters->Read(bResponse);
			parameters->Read(wListItem);
			parameters->Read(bTextLength);
			parameters->Read(Text, bTextLength);
			Text[bTextLength] = '\0';


			//	addMessageToChatWindow(2, "%s [%d]", Text, wDialogID);

		}
	
		if (*uniqueID == RPC_ClientJoin)
		{
			addMessage(2, "İyi Oyunlar");
		}

		if(*uniqueID == RPC_Spawn)
		{
			girisyapildi = true;
		}

		if (!OnSendRPC(*uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp))
			return false;
	}
		

	return g_RakClient->GetInterface()->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
}


#define SILENT_TYPE_HEAD	0
#define SILENT_TYPE_BODY	1
#define SILENT_TYPE_FOOT	2
#define SILENT_TYPE_RANDOM	3

#define TYPE				0
#define TYPE2				1

bool HookedRakClientInterface::Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	
	BYTE packetId;
	bitStream->Read(packetId);


	if (packetId == PacketEnumeration::ID_BULLET_SYNC)
	{
		stBulletData bullet{};
		bitStream->Read((PCHAR)&bullet, sizeof(stBulletData));


		if (silentaim)
		{
			if (aimedplayer != -1)
			{
				if (RangeShot)
				{
					if (kayitliserver)
					{
						bullet.byteType = 0;
					}
					else
						bullet.byteType = 1;
				}
				else
					bullet.byteType = 1;

				bullet.sTargetID = aimedplayer;

				float pos[3];

				vect3_copy(&g_Players->pRemotePlayer[aimedplayer]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[4 * 3], pos);

				bullet.fTarget[0] = pos[0];
				bullet.fTarget[1] = pos[1];
				bullet.fTarget[2] = pos[2];

				bullet.fCenter[0] = random_float(-0.1, 0.1);
				bullet.fCenter[1] = random_float(-0.1, 0.1);
				bullet.fCenter[2] = random_float(-0.1, 0.1);

				if (silent_body_type == SILENT_TYPE_HEAD)
				{
					g_RakClient->SendGiveDamage(aimedplayer, CalculateDamage(), g_Players->pLocalPlayer->byteCurrentWeapon, 9); // HEAD,
				}
				else if (silent_body_type == SILENT_TYPE_BODY)
				{
					g_RakClient->SendGiveDamage(aimedplayer, CalculateDamage(), g_Players->pLocalPlayer->byteCurrentWeapon, 3); // TORSO
				}
				else if (silent_body_type == SILENT_TYPE_FOOT)
				{
					g_RakClient->SendGiveDamage(aimedplayer, CalculateDamage(), g_Players->pLocalPlayer->byteCurrentWeapon, 8); // RIGHT LEG
					g_RakClient->SendGiveDamage(aimedplayer, CalculateDamage(), g_Players->pLocalPlayer->byteCurrentWeapon, 7); // LEFT  LEG
				}
				else if (silent_body_type == SILENT_TYPE_RANDOM)
				{
					std::vector<int> list{ 9, 3, 8, 7 };
					int index = rand() % list.size();
					int value = list[index];

					g_RakClient->SendGiveDamage(aimedplayer, CalculateDamage(), g_Players->pLocalPlayer->byteCurrentWeapon, value);

				}
				
				

				bitStream->Reset();
				bitStream->Write((BYTE)ID_BULLET_SYNC);
				bitStream->Write((PCHAR)&bullet, sizeof(stBulletData));

			

			}
			
		}
	}

	
	
	if (packetId == PacketEnumeration::ID_PLAYER_SYNC)
	{

		stOnFootData onfoot{};
		bitStream->ResetReadPointer();
		bitStream->Read(packetId);
		bitStream->Read((PCHAR)&onfoot, sizeof(stOnFootData));




		if (FakeBullet)
		{
			if (KEY_DOWN(VK_LBUTTON))
			{
				if (IsPlayerFriend(aimedplayer))
					return false;

				g_RakClient->SendGiveDamage(aimedplayer, 2.6400001049041748046875, g_Players->pLocalPlayer->byteCurrentWeapon, 3);
			}
		}

		if (antistun)
		{

			if (onfoot.sCurrentAnimationID == 1087 || onfoot.sCurrentAnimationID == 1177 ||
				onfoot.sCurrentAnimationID == 1078 || onfoot.sCurrentAnimationID == 1073 ||
				onfoot.sCurrentAnimationID == 1079 || onfoot.sCurrentAnimationID == 1077 ||
				onfoot.sCurrentAnimationID == 1081 || onfoot.sCurrentAnimationID == 1075 ||
				onfoot.sCurrentAnimationID == 1084 || onfoot.sCurrentAnimationID == 1083 ||
				onfoot.sCurrentAnimationID == 1085 || onfoot.sCurrentAnimationID == 1083 ||
				onfoot.sCurrentAnimationID == 1086 || onfoot.sCurrentAnimationID == 1072 ||
				onfoot.sCurrentAnimationID == 1175 || onfoot.sCurrentAnimationID == 1076 ||
				onfoot.sCurrentAnimationID == 1074 || onfoot.sCurrentAnimationID == 1082 ||
				onfoot.sCurrentAnimationID == 1173 || onfoot.sCurrentAnimationID == 1178 ||
				onfoot.sCurrentAnimationID == 1087 || onfoot.sCurrentAnimationID == 1078)
			{
				DisembarkInstantly();

			}
		}

		if (ActorSlapper)
		{

			if (IsPlayerFriend(aimedplayer)) //test
				return false;

			int aimed;

			switch (SlapperType)
			{
			case TYPE:
				aimed = aimedplayer;
				break;
			case TYPE2:
				aimed = SAMP_MAX_PLAYERS;
				if (!IsPlayerStreamed(aimed)) return false;
				break;
			}


			if (KEY_DOWN(VK_LBUTTON)) {

				onfoot.fMoveSpeed[0] = g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[0] * 10.0f;
				onfoot.fMoveSpeed[1] = g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[1] * 10.0f;
				onfoot.fMoveSpeed[2] = g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTAEntity->speed[2] * 10.0f;

				onfoot.fPosition[0] += g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[0] * 30.0f;
				onfoot.fPosition[1] += g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[1] * 30.0f;
				onfoot.fPosition[2] += g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTA_Ped->speed[2] * 30.0f;

			}


		}


		if (ActorUPInv)
		{
			if (!onfoot.stSampKeys.keys_secondaryFire__shoot)
			{
				onfoot.fPosition[2] = 700.0f;
				onfoot.fMoveSpeed[2] = 0.4f;
			}

		}



		if (antics)
		{
			onfoot.sSurfingVehicleID = NAN;
			onfoot.fSurfingOffsets[0] = random_float(-1.00000f, 1.10000f);
			onfoot.fSurfingOffsets[1] = random_float(-1.00000f, 1.10000f);
			onfoot.fSurfingOffsets[2] = -35.0f;
		}

		if (afk)
		{
			if (!onfoot.stSampKeys.keys_secondaryFire__shoot)
			{
				onfoot.fQuaternion[2] = NAN;
				onfoot.fPosition[2] = NAN;
			}
		}

		if (FakeLag)
			vect3_zero(onfoot.fMoveSpeed);

		bitStream->Reset();
		bitStream->Write((BYTE)ID_PLAYER_SYNC);
		bitStream->Write((PCHAR)&onfoot, sizeof(stOnFootData));
	}



	if (packetId == PacketEnumeration::ID_AIM_SYNC)
	{
		stAimData aim{};
		bitStream->ResetReadPointer();
		bitStream->Read(packetId);
		bitStream->Read((PCHAR)&aim, sizeof(stAimData));

		if (RangeShot)
		{
			if (!kayitliserver)
			{
				if (IsPlayerShot(g_Players->sLocalPlayerID))
				{
					aim.byteCamMode = 7;
					aim.byteCamExtZoom = 28;
				}
			}

		}
			
		if (silentaim)
		{
			if (kayitliserver)
			{
				if (IsPlayerShot(g_Players->sLocalPlayerID))
				{
					if (aimedplayer != -1)
					{
						actor_info* actor;
						actor = actor_info_get(aimedplayer, 0);

						if(actor)
								vect3_copy(&g_Players->pRemotePlayer[aimedplayer]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[4 * 3], aim.vecAimPos);

						float pos[3];

						if (getPlayerPos(aimedplayer, pos))
						{
							aim.vecAimf1[0] = pos[0];
							aim.vecAimf1[1] = pos[1];
							aim.vecAimf1[2] = random_float(-0.10f, 0.1f);
						}
	
					}

				}
			}

		}
		



		bitStream->Reset();
		bitStream->Write((BYTE)ID_AIM_SYNC);
		bitStream->Write((PCHAR)&aim, sizeof(stAimData));
	}

	if (packetId == PacketEnumeration::ID_VEHICLE_SYNC)
	{
		stInCarData vehicle{};
		bitStream->ResetReadPointer();
		bitStream->Read(packetId);
		bitStream->Read((PCHAR)&vehicle, sizeof(stInCarData));


		if (VehiclePosInv)
		{
			vehicle.fPosition[2] = -500.0f;
			vehicle.fQuaternion[2] = 0.45f;

		}


		if (CarSlapper && aimedplayer != -1)
		{

			if (KEY_DOWN(VK_LBUTTON))
			{
				vehicle.fMoveSpeed[0] = 0.40261f;
				vehicle.fMoveSpeed[1] = 0.40261f;
				vehicle.fMoveSpeed[2] = 0.45559f;


				int aimed;

				switch (SlapperType)
				{
					case TYPE:
						aimed = aimedplayer;
						break;
					case TYPE2:
						aimed = SAMP_MAX_PLAYERS;
						if (!IsPlayerStreamed(aimed)) return false;
						break;
				}

				if (IsPlayerFriend(aimedplayer))
					return false;


				switch (g_Players->pRemotePlayer[aimed]->pPlayerData->bytePlayerState)
				{
					case PLAYER_STATE_ONFOOT:
						vect3_copy(&g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12], vehicle.fPosition);
						break;
					case PLAYER_STATE_DRIVER:
						vect3_copy(&g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle->base.matrix[12], vehicle.fPosition);
						break;
					case PLAYER_STATE_PASSENGER:
						vect3_copy(&g_Players->pRemotePlayer[aimed]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle->base.matrix[12], vehicle.fPosition);
						break;
				}


			}
		}


		bitStream->Reset();
		bitStream->Write((BYTE)ID_VEHICLE_SYNC);
		bitStream->Write((PCHAR)&vehicle, sizeof(stInCarData));

	}
	
	if (bitStream != nullptr)
	{
		if (!OnSendPacket(bitStream, priority, reliability, orderingChannel))
			return false;
	}

	return g_RakClient->GetInterface()->Send(bitStream, priority, reliability, orderingChannel);
}

Packet* HookedRakClientInterface::Receive(void)
{
	Packet* p = g_RakClient->GetInterface()->Receive();
	BYTE GelenPaketID = GetPacketID(p);
	unsigned short OyuncuID;


	if (GelenPaketID == PacketEnumeration::ID_PLAYER_SYNC)
	{
		BitStream playerdata((unsigned char*)p->data, p->length, false);
		stOnFootData onfoot;
		playerdata.IgnoreBits(8);
		playerdata.Read(OyuncuID);
		playerdata.Read((PCHAR)&onfoot, sizeof(stOnFootData));

	}

	if (GelenPaketID == PacketEnumeration::ID_BULLET_SYNC)
	{
		BitStream bsBulletData((unsigned char*)p->data, p->length, false);
		stBulletData BulletData;
		bsBulletData.IgnoreBits(8);
		bsBulletData.Read(OyuncuID);
		bsBulletData.Read((PCHAR)&BulletData, sizeof(stBulletData));



	}

	if (GelenPaketID == PacketEnumeration::ID_AIM_SYNC)
	{
		BitStream bsBulletData((unsigned char*)p->data, p->length, false);
		stAimData AimData;
		bsBulletData.IgnoreBits(8);
		bsBulletData.Read(OyuncuID);
		bsBulletData.Read((PCHAR)&AimData, sizeof(stAimData));
		bsBulletData.Read(AimData.vecAimf1);



	}
	
	while (p != nullptr)
	{
		if (OnReceivePacket(p))
			break;
		g_RakClient->GetInterface()->DeallocatePacket(p);
		p = g_RakClient->GetInterface()->Receive();
	}

	return p;
}
