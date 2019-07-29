#ifndef mI2c_h
#define mI2c_h

#include "driver/gpio.h"
#include "driver/i2c.h"

class mI2cMaster
{
public:
	mI2cMaster(i2c_port_t port, gpio_num_t pinSda, gpio_num_t pinClk, uint8_t deviceAddress, uint32_t frequency);
	~mI2cMaster();

	uint8_t ReadRegister(uint8_t registerAddress);
	uint16_t ReadRegister16(uint8_t registerAddress);

	void WriteRegister(uint8_t registerAddress, uint8_t data);
	void WriteRegister16(uint8_t registerAddress, uint16_t data);

	bool Detect();

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