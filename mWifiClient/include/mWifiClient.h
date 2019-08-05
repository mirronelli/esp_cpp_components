#ifndef mWifiClient_h
#define mWifiClient_h

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h" 
#include "esp_event_base.h"

#include <string>

class mWifiClient
{
private:
	const int WIFI_CONNECTED_BIT = BIT0;
	const char* logTag = "mWifiClient";
	
	std::string ssid;
	std::string password;
	int maxRetries=5;
	int numRetries=0;
	bool isConnected = false;
	bool initialized = false;
	bool explicitDiconectRequested = false;
	EventGroupHandle_t eventGroup;
			
	static void EventHandler(void* instance, esp_event_base_t base, int32_t eventId, void* eventData);
	void Init();

public:
	mWifiClient(std::string ssid, std::string password, int maxRetries);
	bool IsConnected();
	bool Connect(uint16_t timeout);
	void Disconnect();
};

#endif