#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include <stdio.h>	

#include <string>

#include "include/mWifiClient.h"

mWifiClient::mWifiClient(std::string ssid, std::string password, mWifiEventHandler connectHandler, mWifiEventHandler disconnectHandler, void *handlerArg) : 
mWifiClient(ssid, password)
{
	this->connectHandler = connectHandler;
	this->disconnectHandler = disconnectHandler;
	this->handlerArg = handlerArg;
}

mWifiClient::mWifiClient(std::string ssid, std::string password)
{
	this->ssid = ssid;
	this->password = password;
}

void mWifiClient::EventHandler(void *instance, esp_event_base_t base, int32_t eventId, void *eventData)
{
	mWifiClient *client = (mWifiClient *)instance;

	ESP_LOGI(client->logTag, "Got Event: %d", eventId);
	switch (eventId)
	{
	case WIFI_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case IP_EVENT_STA_GOT_IP:
		ESP_LOGI(client->logTag, "GotIP");

		client->isConnected = true;
		xEventGroupSetBits(client->eventGroup, client->WIFI_CONNECTED_BIT);
		
		if (client->connectHandler != nullptr)
		{
			client->connectHandler(client->handlerArg);
		}
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
	{
		client->isConnected = false;

		// intended disconnect
		if (client->explicitDiconectRequested)
		{
			ESP_LOGI(client->logTag, "Explicit disconnect.");
		}
		else 
		{
			ESP_LOGI(client->logTag, "Unexpected disconnect.");
		}
		client->explicitDiconectRequested = false;

		if (client->disconnectHandler != nullptr)
		{
			client->disconnectHandler(client);
		}

		break;
	}
	default:
		break;
	}
}

void mWifiClient::Init()
{
	if (!initialized)
	{
		ESP_LOGI(logTag, "Initializing wifi");
		nvs_flash_init();
		tcpip_adapter_init();

		eventGroup = xEventGroupCreate();
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, mWifiClient::EventHandler, this));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, mWifiClient::EventHandler, this));

		wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&initConfig));
		ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

		wifi_config_t connectConfig = {};
		this->ssid.copy((char *)connectConfig.sta.ssid, sizeof(connectConfig.sta.ssid), 0);
		this->password.copy((char *)connectConfig.sta.password, sizeof(connectConfig.sta.password), 0);

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &connectConfig));

		initialized = true;
	}
}

bool mWifiClient::Connect(uint16_t timeout)
{
	Init();

	ESP_LOGI(logTag, "Connecting to AP: %s", ssid.c_str());

	xEventGroupClearBits(eventGroup, WIFI_CONNECTED_BIT);
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
	xEventGroupWaitBits(eventGroup, WIFI_CONNECTED_BIT, false, true, timeout / portTICK_RATE_MS);

	return isConnected;
}

void mWifiClient::Disconnect()
{
	explicitDiconectRequested = true;
	ESP_ERROR_CHECK(esp_wifi_disconnect());
}

bool mWifiClient::IsConnected()
{
	return isConnected;
}