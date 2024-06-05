#ifndef ONE_NET_MQTT_H
#define ONE_NET_MQTT_H

#include "WiFi.h"
#include <PubSubClient.h>

WiFiClient _wifiClient;
PubSubClient _client(_wifiClient);

class onenetMQTT {

private:
    const char* _ssid;
    const char* _password;
    const char* _mqtt_pubid;
    const char* _mqtt_devid;
    const char* _mqtt_password;

    const char* _mqtt_server = "47.91.203.178";       //Onenet IP address
    const int _mqtt_port = 6002;  


    char msgJson[100];                                
    unsigned short json_len = 0;
    char msg_buf[200];


public:

    onenetMQTT();

    onenetMQTT(const char* ssid, const char* password, const char* pubid, const char* devid, const char* mqttPassword) {
        _ssid = ssid;
        _password = password;
        _mqtt_pubid = pubid;
        _mqtt_devid = devid;
        _mqtt_password = mqttPassword;
    }
    static void callback(char *topic, byte *payload, unsigned int length){ 
      Serial.println("message rev:");
    //   Serial.println(topic);
         for (size_t i = 0; i < length; i++)  {    
           Serial.print((char)payload[i]);
             }  
             Serial.println();    
    }
    bool connect() {
        // 連接 WiFi
        Serial.println("Wifi connecting...");
        unsigned long startTime = millis();
        WiFi.begin(_ssid, _password);  
        while (!WiFi.isConnected())  {
        if (millis() - startTime > 180000) { // 3 minutes timeout
                Serial.println("Timeout");
                break;
            }
        Serial.print(".");    
        delay(500);  
        }  
        Serial.println("OK");  
        Serial.println("Wifi connect success");

        // 連接 MQTT 伺服器

        _client.setServer(_mqtt_server, _mqtt_port);
        _client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            this->onMessageReceived(topic, payload, length);
        });

      _client.connect(_mqtt_devid, _mqtt_pubid, _mqtt_password);
        Serial.print("OneNet is connected to ");
        Serial.println(_mqtt_devid);
        _client.setCallback(callback); // TODO fix recieve feature
       return true;
    }

    bool publish(const char* topic, const char* payload) {
        return _client.publish(topic, payload);
    }

    char* payloadSetup_simple(char* dataTemplate, ...) {
        va_list args;
        va_start(args, dataTemplate);

        // Determine the required size for the final string
        int len = vsnprintf(NULL, 0, dataTemplate, args) + 1; // +1 for null terminator
        va_end(args);
        // Format the data into the result string
        va_start(args, dataTemplate);
        vsnprintf(msgJson, len, dataTemplate, args);
        va_end(args);

        // MQTT header part
        unsigned short json_len = strlen(msgJson);     //length of msgJson
        msg_buf[0] = char(0x03);                       //buffer of the data sent first part is 3
        msg_buf[1] = char(json_len >> 8);              //Second part is the 8 MSB of the data
        msg_buf[2] = char(json_len & 0xff);            //Third part is 8 LSB of the data
        memcpy(msg_buf + 3, msgJson, strlen(msgJson)); //Forth part is the data in msgJson
        msg_buf[3 + strlen(msgJson)] = 0;              //Add a 0 at last
        return msgJson;
    }
    bool publish(const char* payload) {
        if (_client.connected()) {
            _client.publish("$dp", (uint8_t *)msg_buf, 3 + strlen(payload)); //Send data
            Serial.print("public message client:");
            Serial.println(msgJson);
            return false;
        }
        return false;
    }

    void operator ==(const onenetMQTT& other) {
        _ssid = other._ssid;
        _password = other._password;
        _mqtt_pubid = other._mqtt_pubid;
        _mqtt_devid = other._mqtt_devid;
        _mqtt_password = other._mqtt_password;
    }

    void loop() {
        _client.loop();
    }

protected:
    virtual void onMessageReceived(char* topic, byte* payload, unsigned int length) {
        // 可以在此處理收到的 MQTT 訊息
        Serial.print("Received message: ");
        Serial.println((char*)payload);
    }




};





// static void _messageCallback(char *topic, unsigned char* payload, unsigned int length);


#endif  // ONE_NET_MQTT_H