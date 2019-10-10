#include "include/mI2cSlave.h"
#include "exception"
#include "esp_log.h"

mI2cSlave::mI2cSlave(i2c_port_t port, gpio_num_t pinSda, gpio_num_t pinClk, uint8_t deviceAddress, uint32_t frequency, bool enablePullUps)
	: port{port}, pinSda{pinSda}, pinClk{pinClk}, deviceAddress{deviceAddress}, frequency{frequency}
{
	i2c_config_t conf = {
		.mode = I2C_MODE_SLAVE,
		.sda_io_num = pinSda,
		.sda_pullup_en = enablePullUps ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
		.scl_io_num = pinClk,
		.scl_pullup_en = enablePullUps ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
		{.master{
			.clk_speed = frequency}}};
	i2c_param_config(port, &conf);
	i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);
}

mI2cSlave::~mI2cSlave()
{
	i2c_driver_delete(port);
}