/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#include "main.h"



void process(void*)
{
	SAMP::ServerData data;
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);


	Discord::initialize();
	if (SAMP::readServerData(GetCommandLine(), data))
	{
		if (data.connect == SAMP::SAMP_CONNECT_SERVER) {
			SAMP::Query query(data.address, std::stoi(data.port));
			while (true) {
				SAMP::Query::Information information;
				if (query.info(information)) 
					if (data.address == "188.165.45.72" || "91.151.94.174")
                        kayitliserver = true;		
			}
		}



	}
}



void HandleServerRPCPacketFunc(unsigned char id, RPCParameters* rpcParams, void(*callback) (RPCParameters*))
{
	if (rpcParams != nullptr && rpcParams->numberOfBitsOfData >= 8)
	{
		switch (id)
		{
			case RPC_ServerJoin:
			{
				PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
				int iBitLength = rpcParams->numberOfBitsOfData;
				BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);


				short int p_id;
				D3DCOLOR p_color;
				byte p_nameLen, p_isNPC;
				char p_szPlayerName[25];

				bsData.Read(p_id);
				bsData.Read(p_color);
				bsData.Read(p_isNPC);
				bsData.Read(p_nameLen);
				bsData.Read(p_szPlayerName, p_nameLen);
				p_szPlayerName[p_nameLen] = '\0';

				if (girisyapildi)
				{
					logger->addLog("Last Joins: %s - ID: %d - Level: %d \n", p_szPlayerName, p_id, g_Players->iLocalPlayerScore);
				//	addMessageToChatWindow(1, "Last Joins: %s - ID: %d - Level: %d", p_szPlayerName, p_id, g_Players->iLocalPlayerScore);
				}

				break;
			}
			case RPC_Create3DTextLabel:
			{
				short int wLabelID;
				short int wColor;
				float x, y, z;
				float wDrawDistance;
				short int testLOS;
				short int attachedPlayer;
				short int attachedVehicle;
				char text[25];

				PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
				int iBitLength = rpcParams->numberOfBitsOfData;
				BitStream parameters((unsigned char*)Data, (iBitLength / 8) + 1, false);

				parameters.Read(wLabelID);
				parameters.Read(wColor);

				parameters.Read(x);
				parameters.Read(y);
				parameters.Read(z);

				parameters.Read(testLOS);

				parameters.Read(attachedPlayer);
				parameters.Read(attachedVehicle);
				parameters.Read(text);




				{
					if (text != NULL)
					{
					//	addMessageToChatWindow(2, "LabelID: %d, wColor: %d, x: %f, y: %f, z: %f, Distance: %f, attachedPlayer: %d", wLabelID, wColor, x, y, z, wDrawDistance, attachedPlayer);
					//	addMessageToChatWindow(2, "Text: %s", text);
					}

				}

				break;
			}
			case RPC_ShowTextDraw:
			{
				WORD TextID;
				char cText[1024];
				unsigned short cTextLen = 0;

				BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);

				break;
			}
			case RPC_ShowDialog:
			{

				PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
				int iBitLength = rpcParams->numberOfBitsOfData;
				BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);


				
				break;
			}
			case RPC_ClientMessage:
			{
				PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
				int iBitLength = rpcParams->numberOfBitsOfData;
				BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

				char messsage[400];
				UINT32 dColor;
				UINT32 dMessageLength;

				bsData.Read(dColor);
				bsData.Read(dMessageLength);

				bsData.Read(messsage, dMessageLength);
				messsage[dMessageLength] = '\0';

				std::string message_key = std::string(messsage);

				break;
			}
		}
	}

	callback(rpcParams);
}

