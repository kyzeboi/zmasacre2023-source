#pragma once


namespace Discord
{
	void initialize();
	void update(const time_t time, const char* state, const char* details, const char* image, const char* imageDetails, const char* smallImageKey, const char* infoDetails, ...);

	inline void update(const time_t time, const char* state, const char* details, const char* image, const std::string& imageDetails, const std::string& smallImageKey, const std::string& infoDetails)
	{
		return update(time, state, details, image, imageDetails.c_str(), smallImageKey.c_str(), infoDetails.c_str());
	}
}


