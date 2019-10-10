#ifndef mI2cSlave_h
#define mI2cSlave_h

#include "driver/gpio.h"
#include "driver/i2c.h"

class mI2cSlave
{
public:
	mI2cSlave(i2c_port_t port, gpio_num_t pinSda, gpio_num_t pinClk, uint8_t deviceAddress, uint32_t frequency, bool enablePullUps);
	~mI2cSlave();

private:
	i2c_port_t port;
	gpio_num_t pinSda;
	gpio_num_t pinClk;
	uint8_t deviceAddress;
	uint32_t frequency;

	const char *logTag = "mI2c";
	const bool ACK_REQUIRED = true;
	const bool ACK_NOTREQURED = false;
};

#endif