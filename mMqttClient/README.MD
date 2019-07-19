# mMqttClient
A simple c++ mqtt client with send and subscribe functionality.


sample:
```c++
// create an instance with broker address
mqttClient = new mMqttClient(mqttBrokerAddress);

// subscribe to a topic topic_name, qos=1 and passing a handler
mqttClient->Subscribe("topic_name", 1, YourClass::MessageHandler);

// publish on a topic passsing topic, qos, retain and the message itself
int message = mqttClient->Publish("topic_name", 1, 0, "Hello World!");

// stop and unsubscribe
mqttClient->stop();
```