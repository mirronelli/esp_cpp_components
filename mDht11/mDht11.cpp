#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <exception>

#include "mDht11.h"

mDht11Sensor::mDht11Sensor(gpio_num_t pin)
{
	this->pin = pin;
}

mDht11Measure mDht11Sensor::Read()
{
	mDht11Measure result;
	int64_t time = esp_timer_get_time();
	if (time < 2000000){
		vTaskDelay((2000 - time/1000ULL)/portTICK_PERIOD_MS);
	} else if (time - lastResultTime < 2000000) 
	{
		ESP_LOGV(logTag, "Returning early with last result since not enough time has passed for the sensor to respond again.");
		return lastResult;
	}

	try
	{
		SendReset();
		WaitForPresence();
		result = ReadData();
	}
	catch (void *error)
	{
		ESP_LOGI("DHT11", "O oooo");
		throw;
	}

	return result;
}

void mDht11Sensor::SendReset()
{
	gpio_set_direction(this->pin, GPIO_MODE_OUTPUT);

	// pull down for 20 ms
	gpio_set_level(this->pin, 0);
	ets_delay_us(20000);

	// pull up for 20 us
	gpio_set_level(this->pin, 1);
	ets_delay_us(20);
}

void mDht11Sensor::WaitForPresence()
{
	gpio_set_direction(this->pin, GPIO_MODE_INPUT);

	if (WaitForState(1, 100) == -1)
		throw TimeoutException(1);
	if (WaitForState(0, 100) == -1)
		throw TimeoutException(2);
}

mDht11Measure mDht11Sensor::ReadData()
{
	mDht11Measure result = {0, 0};
	uint8_t buffer[5] = {0, 0, 0, 0, 0};

	for (int i = 0; i < 40; i++)
	{
		if (WaitForState(1, 60) == -1)
			throw TimeoutException(3);
		if (WaitForState(0, 80) > 28)
			buffer[i / 8] |= (1 << (7 - (i % 8)));
	}

	CheckCrc(buffer);

	result.Humidity = buffer[0];
	result.Temperature = buffer[2] + buffer[3]/10.0;
	lastResult = result;
	lastResultTime = esp_timer_get_time();
	return result;
}

int mDht11Sensor::WaitForState(int state, int timeout)
{
	int microCounter = 0;
	while (microCounter++ < timeout && gpio_get_level(this->pin) != state)
		ets_delay_us(1);

	return microCounter >= timeout ? -1 : microCounter;
}

void mDht11Sensor::CheckCrc(uint8_t data[5])
{
	ESP_LOGV(logTag, "%d\t%d\t%d\t%d\t %d = %d on core:%d\n", data[0], data[1], data[2], data[3], data[4], data[0] + data[1] + data[2] + data[3], xPortGetCoreID());
	if (data[4] != (data[0] + data[1] + data[2] + data[3]))
		throw CrcException();
}