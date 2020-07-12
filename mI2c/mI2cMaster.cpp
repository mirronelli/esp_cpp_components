#include "include/mI2cMaster.h"
#include "exception"
#include "esp_log.h"

mI2cMaster::mI2cMaster(i2c_port_t port, gpio_num_t pinSda, gpio_num_t pinClk, uint8_t deviceAddress, uint32_t frequency, bool enablePullUps)
	: port{port}, pinSda{pinSda}, pinClk{pinClk}, deviceAddress{deviceAddress}, frequency{frequency}
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = pinSda,
		.sda_pullup_en = enablePullUps ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
		.scl_io_num = pinClk,
		.scl_pullup_en = enablePullUps ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
		{
			.master
			{
				.clk_speed = frequency
			}
		}
	};
	i2c_param_config(port, &conf);
	i2c_driver_install(port, I2C_MODE_MASTER, 0, 0, 0);

	int a, b;

	i2c_get_period(port, &a, &b);
	ESP_LOGI(logTag, "High period %d, low period %d", a, b);

	i2c_get_start_timing(port, &a, &b);
	ESP_LOGI(logTag, "Start timing. Setup timing %d, Hold time %d", a, b);

	i2c_get_stop_timing(port, &a, &b);
	ESP_LOGI(logTag, "Stop timing. Setup timing %d, Hold time %d", a, b);

	i2c_get_data_timing(port, &a, &b);
	ESP_LOGI(logTag, "Data timing. Sample timing %d, Hotd time %d", a, b);

	i2c_get_timeout(port, &a);
	ESP_LOGI(logTag, "Timeout %d", a);
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

uint16_t mI2cMaster::ReadRegister16(uint8_t registerAddress)
{
	ESP_LOGI(logTag, "Read from i2c register %d",  registerAddress);
	uint8_t data1 = 0;
	uint8_t data2 = 0;
	uint16_t resultData = 0;

	i2c_cmd_handle_t cmd;
	esp_err_t result;

	cmd = i2c_cmd_link_create();

	// Write register address to device
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_write_byte(cmd, registerAddress, ACK_REQUIRED);
	i2c_master_stop(cmd);
	result = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	ESP_LOGI(logTag, "Write result: %d", result);

	cmd = i2c_cmd_link_create();
	// Read from register
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_READ, ACK_REQUIRED);
	i2c_master_read_byte(cmd, &data1, I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, &data2, I2C_MASTER_NACK);
	i2c_master_stop(cmd);


	// Execute queue and destroy cmd
	result = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);
	ESP_LOGI(logTag, "Read result: %d", result);

	resultData = data1 << 8 | data2;

	if (result != ESP_OK)
		throw std::exception();

	return resultData;
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

void mI2cMaster::WriteRegister16(uint8_t registerAddress, uint16_t data)
{
	i2c_cmd_handle_t cmd;
	esp_err_t result;

	uint8_t msb = data >> 8;
	uint8_t lsb = data & 0xff;

	cmd = i2c_cmd_link_create();

	// Prepare statements
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_write_byte(cmd, registerAddress, ACK_REQUIRED);
	i2c_master_write_byte(cmd, msb, ACK_REQUIRED);
	i2c_master_write_byte(cmd, lsb, ACK_REQUIRED);
	i2c_master_stop(cmd);

	// Execute queue and destroy cmd
	result = i2c_master_cmd_begin(port, cmd, 50 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	
	if (result == ESP_OK){
		ESP_LOGI(logTag, "Written %x to register %d with result %d", data, registerAddress, result);
	}
	else {
		ESP_LOGI("mI2c", "Error writing register: %d", result);
		throw std::exception();
	}
}

bool mI2cMaster::Detect()
{
	i2c_cmd_handle_t cmd;
	esp_err_t result;

	cmd = i2c_cmd_link_create();

	// Write register address to device anb wait for ack
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_stop(cmd);

	// Execute queue and destroy cmd
	result = i2c_master_cmd_begin(port, cmd, 50 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (result != ESP_OK)
	{
		ESP_LOGI("mI2c", "Device NOT detected: %d", deviceAddress);
		return false;
	}
	else
	{
		ESP_LOGI("mI2c", "Device detected: %d", deviceAddress);
		return true;
	}
}

/* This is a non working version. Do not use. */
void mI2cMaster::WriteData(size_t size, uint8_t* data)
{
	i2c_cmd_handle_t cmd;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (deviceAddress << 1) | I2C_MASTER_WRITE, ACK_REQUIRED);
	i2c_master_write(cmd, data, size, ACK_REQUIRED);
	i2c_master_stop(cmd);
}