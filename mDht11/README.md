# mDHT
Implements one wire communication to talk to DHT11. DHT11 is a slow sensor and cannot provide reading more than once in a 2 second period. mDht overcomes this by caching the last value and providing it if asked for new reading sooner than 2 seconds after the last reading.

sample:
```c++
// create instance on pin
dht = new mDht11Sensor(pin);
data = dht->Read();
int temp = data.Temperature;
int humidity = data.Humidity;
```