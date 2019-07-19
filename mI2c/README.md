# mI2c
Ultra lightweight I2C client. Does what you would expect and nothing more. Reads or writes registers from an I2C device identified by an address.

sample:
```c++
// create instance on port 0 or 1, set SDA and CLK pins, address and frequency
i2cMaster = new mI2cMaster(port, pinSda, pinClk, deviceAddress, frequency);

// read value from a register	
uint8_t data = i2cMaster->ReadRegister(registerAddress);

//write data to a register
i2cMaster->WriteRegister(registerAddress, data);
```