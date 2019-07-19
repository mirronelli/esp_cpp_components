#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sntp.h"

#include <string>

#include "mTime.h"

static const char *logTag = "mTime";
static std::string sntpServer = "pool.ntp.org";
static const EventGroupHandle_t eventGroup = xEventGroupCreate();

void mTime::SetTimeZone(std::string timeZone)
{
	setenv("TZ", timeZone.c_str(), 1);
	tzset();
}

std::string mTime::GetTimeString(mDateTime dateTime)
{
	char buffer[64];
	strftime(buffer, sizeof(buffer), "%c", &dateTime.Info);
	return std::string(buffer);
}

mDateTime mTime::GetTime()
{
	mDateTime dateTime;
	time(&dateTime.Now);
	localtime_r(&dateTime.Now, &dateTime.Info);

	// Is time set? If not, tm_year will be (1970 - 1900).
	if (dateTime.Info.tm_year < 100)
	{
		ESP_LOGI(logTag, "Time is not synced yet. Syncing.");
		SyncTime();
		time(&dateTime.Now);
		localtime_r(&dateTime.Now, &dateTime.Info);
	}
	return dateTime;
}

void mTime::SyncTime(void)
{
	ESP_LOGI(logTag, "Initializing SNTP");
	xEventGroupClearBits(eventGroup, SNTP_SYNCED_BIT);

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, (char *)sntpServer.c_str());
	sntp_set_time_sync_notification_cb(mTime::EventHandler);
	sntp_init();

	ESP_LOGI(logTag, "Waiting for system time to be set..");
	if ((xEventGroupWaitBits(eventGroup, SNTP_SYNCED_BIT, true, true, 10000 / portTICK_RATE_MS) & SNTP_SYNCED_BIT) == SNTP_SYNCED_BIT)
	{
		ESP_LOGI(logTag, "Time synced succesfully.");
	}
}

void mTime::EventHandler(struct timeval *timeValue)
{
	xEventGroupSetBits(eventGroup, SNTP_SYNCED_BIT);
}