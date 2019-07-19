#include "include/mI2c.h"
#include "exception"

mI2cMaster::mI2cMaster(i2c_port_t port, gpio_num_t pinSda, gpio_num_t pinClk, uint8_t deviceAddress, uint32_t frequency)
	: port{port}, pinSda{pinSda}, pinClk{pinClk}, deviceAddress{deviceAddress}, frequency{frequency}
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = pinSda,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_io_num = pinClk,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		{.master{
			.clk_speed = frequency}}};
	i2c_param_config(port, &conf);
	i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);
}

mI2cMaster::~mI2cMaster()
{
	i2c_driver_delete(port);
}

uint8_t mI2cMaster::ReadRegister(uint8_t registerAddress)
{
	uint8_t data = 0;
	i2c_cmd_handle_t cmd;
	esp_err_t result;

	cmd = i2c_cmd_link_create();

	// Write register address to device
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_write_byte(cmd, registerAddress, ACK_REQUIRED);

	// Read from register
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_READ, ACK_REQUIRED);
	i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);

	// Write stop sequence
	i2c_master_stop(cmd);

	// Execute queue and destroy cmd
	result = i2c_master_cmd_begin(port, cmd, 50 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (result != ESP_OK)
	throw std::exception();

	return data;
}

void mI2cMaster::WriteRegister(uint8_t registerAddress, uint8_t data)
{
	i2c_cmd_handle_t cmd;
	esp_err_t result;

	cmd = i2c_cmd_link_create();

	// Write register address to device
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_write_byte(cmd, registerAddress, ACK_REQUIRED);
	i2c_master_write_byte(cmd, data, ACK_REQUIRED);

	// Write stop sequence
	i2c_master_stop(cmd);

	// Execute queue and destroy cmd
	result = i2c_master_cmd_begin(port, cmd, 50 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (result != ESP_OK)
		throw std::exception();
}
