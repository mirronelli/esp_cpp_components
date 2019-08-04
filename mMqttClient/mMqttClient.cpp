#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include <string>
#include <memory>

#include "mMqttClient.h"

using namespace std;
//static const char *TAG_mqtt = "mqtt";

mMqttClient::mMqttClient(std::string brokerAddress)
{
	this->brokerAddress = brokerAddress;
	eventGroup = xEventGroupCreate();

	esp_mqtt_client_config_t cfg = {};
	cfg.uri = brokerAddress.c_str();
	cfg.event_handle = this->EventHandler;
	cfg.user_context = this;

	clientHandle = esp_mqtt_client_init(&cfg);
	esp_mqtt_client_start(clientHandle);
	xEventGroupWaitBits(eventGroup, MQTT_CONNECTED_BIT, false, true, 10000);
}

esp_err_t mMqttClient::EventHandler(esp_mqtt_event_handle_t event)
{
	mMqttClient *client = (mMqttClient *)event->user_context;

	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(client->logTag, "MQTT_EVENT_CONNECTED");
		xEventGroupSetBits(client->eventGroup, MQTT_CONNECTED_BIT);
		client->isConnected = true;
		break;
	case MQTT_EVENT_DISCONNECTED:
		client->isConnected = false;
		break;
	case MQTT_EVENT_DATA:
	{
		ESP_LOGI(client->logTag, "MQTT_EVENT_DATA");
		std::string topic(event->topic, event->topic_len);
		string message(event->data, event->data_len);
		client->DispatchMessage(topic, message);
		break;
	}
	default:
		break;
	}

	return ESP_OK;
}

int mMqttClient::Publish(std::string topic, int qos, int retain, std::string message)
{
	ESP_LOGI(logTag, "Publishing to %s on %s\n%s", brokerAddress.c_str(), topic.c_str(), message.c_str());
	return esp_mqtt_client_publish(this->clientHandle, topic.c_str(), message.c_str(), 0, qos, retain);
}

int mMqttClient::PublishFormat(std::string topic, int qos, int retain, std::string format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);

	return Publish(topic, qos, retain, std::string(buffer));
}

void mMqttClient::Stop()
{
	if (isConnected)
	{
		isConnected = false;
		ESP_ERROR_CHECK(esp_mqtt_client_stop(clientHandle));
	}
	handlers.clear();
}

bool mMqttClient::IsConnected()
{
	return isConnected;
}

void mMqttClient::Subscribe(std::string topic, int qos, messageHandler handler, void * arg)
{
	esp_mqtt_client_subscribe(this->clientHandle, topic.c_str(), qos);
	
	mMqttHandler handlerStruct;
	handlerStruct.arg = arg;
	handlerStruct.handler = handler;

	handlers.insert_or_assign(topic, handlerStruct);
}

void mMqttClient::DispatchMessage(string topic, string message)
{
	ESP_LOGI(logTag, "Looking for handler %s ...", topic.c_str());
	if (handlers.count(topic) > 0)
	{
		ESP_LOGI(logTag, "Found handler, dipatching...");
		mMqttHandler handlerStruct = handlers[topic];
		handlerStruct.handler(topic, message, handlerStruct.arg);
	}
}