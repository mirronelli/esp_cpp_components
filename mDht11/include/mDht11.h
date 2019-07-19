#ifndef mDht11_h
#define mDht11_h

#include <exception>
#include "driver/gpio.h"

struct mDht11Measure
{
	float Temperature;
	uint8_t Humidity;
};

class mDht11Sensor
{
	public:
		mDht11Sensor(gpio_num_t pin);
		mDht11Measure Read();
	private:
		const char *logTag = "mDht11";
		gpio_num_t pin;
		void SendReset();
		void WaitForPresence();
		void DelayMiliseconds(int miliSeconds);
		int WaitForState(int state, int timeout);
		mDht11Measure ReadData();
		mDht11Measure lastResult;
		int64_t lastResultTime = 0;

		void CheckCrc(uint8_t data[5]);
};

struct CrcException : public std::exception
{
	const char *what()
	{
		return "Crc Exception";
	}
};

class TimeoutException : public std::exception
{
	public:
	TimeoutException(int where)
	{
		this->where = where;
	}
	const char *what()
	{
		return "Timeout Exception";
	}
	int where;
};
#endif