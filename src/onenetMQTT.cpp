#include "onenetMQTT.h"

onenetMQTT::onenetMQTT()
{
    _ssid = NULL;
    _password = NULL;
    
    Serial.println("Created MQTT client, but info not setted yet!");

}