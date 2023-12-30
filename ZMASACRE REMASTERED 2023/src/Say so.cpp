/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#include "main.h"

MyRakClient* g_RakClient = NULL;

#define RAKNET_MTU_SIZE			 576
MyRakClient::MyRakClient(void* pRakClientInterface)
{
	pRakClient = (MyRakClientInterface*)pRakClientInterface;
}

bool MyRakClient::RPC(int rpcId, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (!pRakClient)
		return false;

	return pRakClient->RPC(&rpcId, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

bool MyRakClient::Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (!pRakClient)
		return false;

	return pRakClient->Send(bitStream, priority, reliability, orderingChannel);
}

// misc functions
void MyRakClient::SendDeath(uint16_t killerId, uint8_t reason)
{
	BitStream bsDeath;

	bsDeath.Write(reason);
	bsDeath.Write(killerId);
	g_RakClient->RPC(RPC_Death, &bsDeath);
}

void MyRakClient::SendDialogTextResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char* szInputResp)
{
	BYTE respLen = (BYTE)strlen(szInputResp);
	BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInputResp, respLen);
	g_RakClient->RPC(RPC_DialogResponse, &bsSend);

}

void MyRakClient::SendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem)
{
	BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	g_RakClient->RPC(RPC_DialogResponse, &bsSend);

}

void MyRakClient::SendGiveDamage(int iPlayerID, float fDamage, int iWeaponID, int iBodyPart)
{
	BitStream bsGiveDamage;

	bsGiveDamage.Write<bool>(false);
	bsGiveDamage.Write<WORD>(iPlayerID);
	bsGiveDamage.Write<float>(fDamage);
	bsGiveDamage.Write<int>(iWeaponID);
	bsGiveDamage.Write<int>(iBodyPart);
	g_RakClient->RPC(RPC_GiveTakeDamage, &bsGiveDamage);
}

void MyRakClient::SendPickUp(int pickupId)
{
	BitStream bsPickup;

	bsPickup.Write(pickupId);

	RPC(RPC_PickedUpPickup, &bsPickup);
}

void MyRakClient::RequestClass(int classId)
{
	BitStream bsClass;

	bsClass.Write(classId);
	g_RakClient->RPC(RPC_RequestClass, &bsClass);
}

void MyRakClient::SendSCMEvent(int vehicleID, int eventId, int param1, int param2)
{
	BitStream bsScmEvent;

	bsScmEvent.Write(vehicleID);
	bsScmEvent.Write(param1);
	bsScmEvent.Write(param2);
	bsScmEvent.Write(eventId);

	RPC(RPC_SCMEvent, &bsScmEvent);
}

void MyRakClient::SendSpawn(void)
{
	BitStream bsSpawn;

	g_RakClient->RPC(RPC_RequestSpawn, &bsSpawn);
	g_RakClient->RPC(RPC_Spawn, &bsSpawn);
}

void MyRakClient::PlayerPlaySound(int soundid, float x, float y, float z)
{
	BitStream music;
	music.Write(soundid);
	music.Write(x);
	music.Write(y);
	music.Write(z);
	g_RakClient->RPC(16, &music);
}

void MyRakClient::NotifyVehicleDeath(int VehicleID)
{
	BitStream bsDeath;
	bsDeath.Write(VehicleID);
	g_RakClient->RPC(RPC_VehicleDestroyed, &bsDeath);
}


bool HookedRakClientInterface::Connect(const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer)
{
	return g_RakClient->GetInterface()->Connect(host, serverPort, clientPort, depreciated, threadSleepTimer);
}

void HookedRakClientInterface::Disconnect(unsigned int blockDuration, unsigned char orderingChannel)
{
	g_RakClient->GetInterface()->Disconnect(blockDuration, orderingChannel);
}

void HookedRakClientInterface::InitializeSecurity(const char* privKeyP, const char* privKeyQ)
{
	g_RakClient->GetInterface()->InitializeSecurity(privKeyP, privKeyQ);
}

void HookedRakClientInterface::SetPassword(const char* _password)
{
	g_RakClient->GetInterface()->SetPassword(_password);
}

bool HookedRakClientInterface::HasPassword(void) const
{


	return g_RakClient->GetInterface()->HasPassword();
}

bool HookedRakClientInterface::Send(const char* data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{


	return g_RakClient->GetInterface()->Send(data, length, priority, reliability, orderingChannel);
}

void HookedRakClientInterface::DeallocatePacket(Packet* packet)
{


	g_RakClient->GetInterface()->DeallocatePacket(packet);
}

void HookedRakClientInterface::PingServer(void)
{


	g_RakClient->GetInterface()->PingServer();
}

void HookedRakClientInterface::PingServer(const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections)
{


	g_RakClient->GetInterface()->PingServer(host, serverPort, clientPort, onlyReplyOnAcceptingConnections);
}

int HookedRakClientInterface::GetAveragePing(void)
{


	return g_RakClient->GetInterface()->GetAveragePing();
}

int HookedRakClientInterface::GetLastPing(void) const
{


	return g_RakClient->GetInterface()->GetLastPing();
}

int HookedRakClientInterface::GetLowestPing(void) const
{


	return g_RakClient->GetInterface()->GetLowestPing();
}

int HookedRakClientInterface::GetPlayerPing(const PlayerID playerId)
{


	return g_RakClient->GetInterface()->GetPlayerPing(playerId);
}

void HookedRakClientInterface::StartOccasionalPing(void)
{


	g_RakClient->GetInterface()->StartOccasionalPing();
}

void HookedRakClientInterface::StopOccasionalPing(void)
{


	g_RakClient->GetInterface()->StopOccasionalPing();
}

bool HookedRakClientInterface::IsConnected(void) const
{


	return g_RakClient->GetInterface()->IsConnected();
}

unsigned int HookedRakClientInterface::GetSynchronizedRandomInteger(void) const
{


	return g_RakClient->GetInterface()->GetSynchronizedRandomInteger();
}

bool HookedRakClientInterface::GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer)
{


	return g_RakClient->GetInterface()->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
}

bool HookedRakClientInterface::DeleteCompressionLayer(bool inputLayer)
{

	return g_RakClient->GetInterface()->DeleteCompressionLayer(inputLayer);
}

void HookedRakClientInterface::RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters* rpcParms))
{


	g_RakClient->GetInterface()->RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
}

void HookedRakClientInterface::RegisterClassMemberRPC(int* uniqueID, void* functionPointer)
{


	g_RakClient->GetInterface()->RegisterClassMemberRPC(uniqueID, functionPointer);
}

void HookedRakClientInterface::UnregisterAsRemoteProcedureCall(int* uniqueID)
{


	g_RakClient->GetInterface()->UnregisterAsRemoteProcedureCall(uniqueID);
}

bool HookedRakClientInterface::RPC(int* uniqueID, const char* data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{


	return g_RakClient->GetInterface()->RPC(uniqueID, data, bitLength, priority, reliability, orderingChannel, shiftTimestamp);
}

void HookedRakClientInterface::SetTrackFrequencyTable(bool b)
{


	g_RakClient->GetInterface()->SetTrackFrequencyTable(b);
}

bool HookedRakClientInterface::GetSendFrequencyTable(unsigned int outputFrequencyTable[256])
{


	return g_RakClient->GetInterface()->GetSendFrequencyTable(outputFrequencyTable);
}

float HookedRakClientInterface::GetCompressionRatio(void) const
{


	return g_RakClient->GetInterface()->GetCompressionRatio();
}

float HookedRakClientInterface::GetDecompressionRatio(void) const
{


	return g_RakClient->GetInterface()->GetDecompressionRatio();
}

void HookedRakClientInterface::AttachPlugin(void* messageHandler)
{


	g_RakClient->GetInterface()->AttachPlugin(messageHandler);
}

void HookedRakClientInterface::DetachPlugin(void* messageHandler)
{


	g_RakClient->GetInterface()->DetachPlugin(messageHandler);
}

BitStream* HookedRakClientInterface::GetStaticServerData(void)
{


	return g_RakClient->GetInterface()->GetStaticServerData();
}

void HookedRakClientInterface::SetStaticServerData(const char* data, const int length)
{


	g_RakClient->GetInterface()->SetStaticServerData(data, length);
}

BitStream* HookedRakClientInterface::GetStaticClientData(const PlayerID playerId)
{


	return g_RakClient->GetInterface()->GetStaticClientData(playerId);
}

void HookedRakClientInterface::SetStaticClientData(const PlayerID playerId, const char* data, const int length)
{


	g_RakClient->GetInterface()->SetStaticClientData(playerId, data, length);
}

void HookedRakClientInterface::SendStaticClientDataToServer(void)
{


	g_RakClient->GetInterface()->SendStaticClientDataToServer();
}

PlayerID HookedRakClientInterface::GetServerID(void) const
{


	return g_RakClient->GetInterface()->GetServerID();
}

PlayerID HookedRakClientInterface::GetPlayerID(void) const
{


	return g_RakClient->GetInterface()->GetPlayerID();
}

PlayerID HookedRakClientInterface::GetInternalID(void) const
{


	return g_RakClient->GetInterface()->GetInternalID();
}

const char* HookedRakClientInterface::PlayerIDToDottedIP(const PlayerID playerId) const
{


	return g_RakClient->GetInterface()->PlayerIDToDottedIP(playerId);
}

void HookedRakClientInterface::PushBackPacket(Packet* packet, bool pushAtHead)
{


	g_RakClient->GetInterface()->PushBackPacket(packet, pushAtHead);
}

void HookedRakClientInterface::SetRouterInterface(void* routerInterface)
{


	g_RakClient->GetInterface()->SetRouterInterface(routerInterface);
}
void HookedRakClientInterface::RemoveRouterInterface(void* routerInterface)
{


	g_RakClient->GetInterface()->RemoveRouterInterface(routerInterface);
}

void HookedRakClientInterface::SetTimeoutTime(RakNetTime timeMS)
{

	g_RakClient->GetInterface()->SetTimeoutTime(timeMS);
}

bool HookedRakClientInterface::SetMTUSize(int size)
{


	return g_RakClient->GetInterface()->SetMTUSize(size);
}

int HookedRakClientInterface::GetMTUSize(void) const
{


	return g_RakClient->GetInterface()->GetMTUSize();
}

void HookedRakClientInterface::AllowConnectionResponseIPMigration(bool allow)
{


	g_RakClient->GetInterface()->AllowConnectionResponseIPMigration(allow);
}

void HookedRakClientInterface::AdvertiseSystem(const char* host, unsigned short remotePort, const char* data, int dataLength)
{


	g_RakClient->GetInterface()->AdvertiseSystem(host, remotePort, data, dataLength);
}

RakNetStatisticsStruct* const HookedRakClientInterface::GetStatistics(void)
{


	return g_RakClient->GetInterface()->GetStatistics();
}

void HookedRakClientInterface::ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance)
{


	g_RakClient->GetInterface()->ApplyNetworkSimulator(maxSendBPS, minExtraPing, extraPingVariance);
}

bool HookedRakClientInterface::IsNetworkSimulatorActive(void)
{

	return g_RakClient->GetInterface()->IsNetworkSimulatorActive();
}

PlayerIndex HookedRakClientInterface::GetPlayerIndex(void)
{


	return g_RakClient->GetInterface()->GetPlayerIndex();
}

bool HookedRakClientInterface::RPC_(int* uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID)
{


	return g_RakClient->GetInterface()->RPC_(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID);
}