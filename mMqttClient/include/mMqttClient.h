#ifndef mMqttClient_h
#define mMqttClient_h

#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string>
#include <map>
#include <memory>

typedef void (*messageHandler)(std::string topic, std::string message, void *arg);

class mMqttClient
{
	struct mMqttMessage
	{
		std::string Topic;
		std::string Message;
		int Qos = 1;
		int Retain = 0;
	};

	struct mMqttHandler
	{
		void *arg;
		messageHandler handler;
	};

public:
	mMqttClient(std::string brokerAddress);
	int Publish(std::string topic, int qos, int retain, std::string message);
	int PublishFormat(std::string topic, int qos, int retain, std::string format, ...);
	void Subscribe(std::string topic, int qos, messageHandler handler, void* arg);
	void Stop();
	bool IsConnected();

private:
	bool isConnected = false;
	std::map<std::string, mMqttHandler> handlers;
	EventGroupHandle_t eventGroup;
	esp_mqtt_client_handle_t clientHandle;
	std::string brokerAddress;
	const char *logTag = "mMqtt";
	static const int MQTT_CONNECTED_BIT = BIT0;
	static esp_err_t EventHandler(esp_mqtt_event_handle_t event);
	void DispatchMessage(std::string topic, std::string message);
};

#endif