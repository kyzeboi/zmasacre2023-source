/*

	 ** Bu proje kyze. tarafından geliştirilmektedir. **

*/


#include "main.h"

namespace Discord
{
	static void ready(const DiscordUser* request) {}

	static void errored(int errorCode, const char* message) {}

	static void disconnected(int errorCode, const char* message) {}

	static void joinGame(const char* joinSecret) {}

	static void spectateGame(const char* spectateSecret) {}

	static void joinRequest(const DiscordUser* request) {}

	void initialize()
	{
		DiscordEventHandlers handlers = { 0 };
		handlers.ready = ready;
		handlers.errored = errored;
		handlers.disconnected = disconnected;
		handlers.joinGame = joinGame;
		handlers.spectateGame = spectateGame;
		handlers.joinRequest = joinRequest;
		Discord_Initialize("your_application_key_here__kyze", &handlers, 1, nullptr);
		Discord_ClearPresence(); 
	}                            

	void update(const time_t time, const char* state, const char* details, const char* BuyukImage, const char* BuyukImText, const char* KucukImage, const char* KucukImText, ...)
	{
		
		{
			DiscordRichPresence discordPresence = { 0 };
			discordPresence.state = state;
			discordPresence.details = details;
			discordPresence.startTimestamp = time;
			discordPresence.largeImageKey = BuyukImage;
			discordPresence.largeImageText = BuyukImText;
			discordPresence.smallImageKey = KucukImage;
			discordPresence.smallImageText = KucukImText;


			Discord_UpdatePresence(&discordPresence);
		}
	}
}
