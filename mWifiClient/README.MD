# mWifiClient
A simple c++ wifi client with automatic reconnect capability.
If connection gets lost it will automatically try to reconnect for up to specified number of times.

Does not yet read config from kconfig/sdkconfig.h, supply the values to constructor.

sample:
```c++
// create an instance with ssid, password and retry count
wifiClient = new mWifiClient(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, 5);

// try to connect for up to 5000 miliseconds
bool connectSuccesful = wifiClient->Connect(5000);

// check connection status later
bool isConnected = wifiClient->IsConnected();

// disconnect when not needed
wifiClient->Disconnect();
```