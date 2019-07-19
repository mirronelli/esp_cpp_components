#ifndef mTime_h
#define mTime_h

#include "time.h"
#include <string>

struct mDateTime {
	time_t Now;
	struct tm Info;
};

class mTime{
	public:
		static void SetTimeZone(std::string);
		static std::string GetTimeString(mDateTime dateTime);
		static mDateTime GetTime();

	private:
		static void EventHandler(struct timeval *timeValue);
		static void SyncTime();
		static void SetupSntp();
		static void CallBack(struct timeval *tv);

		static const int SNTP_SYNCED_BIT = BIT0;
};

#endif