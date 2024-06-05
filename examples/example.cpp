#include <Arduino.h>

#include "Ticker.h"
#include "Wire.h"
#include <Adafruit_AHTX0.h>
#include <onenetMQTT.h>


const char *ssid = "";   //wifi name
const char *password = "";       //wifi password
#define mqtt_pubid ""                    //Product ID
#define mqtt_devid ""                   //Device ID 
#define mqtt_password ""                 //Device Password 

Adafruit_AHTX0 aht;

char dataTemplate[] = "{\"temperature\":%.2f,\"humidity\":%.2f,\"light\":%d}"; //Data template
Ticker tim1;

onenetMQTT mqtt; // init the global mqtt client
long prev_time = 0; // save for the previous time

void setup(){
    pinMode(35,INPUT);
    pinMode(4,OUTPUT);
    pinMode(2,OUTPUT);
    digitalWrite(4,1);
    //initial IIC
    Wire.begin(14,13);
    //Initialize port                      
    Serial.begin(115200);
    if (aht.begin()) {
    Serial.println("Found AHT20");
  } else {
    Serial.println("Didn't find AHT20");
  }  

  // Connect to Wifi and OneNet
  mqtt == onenetMQTT(ssid, password, mqtt_pubid,mqtt_devid,mqtt_password);
  if (mqtt.connect()) {
    Serial.println("OK");
  }
}

void loop() {

    if(millis() - prev_time > 10000){

    sensors_event_t humidity, temp;   
    aht.getEvent(&humidity, &temp);
    digitalWrite(2,1); // blink LED
    delay(1000);
    digitalWrite(2,0);
    int light = analogRead(35);
    Serial.println("Light: "+light);
    mqtt.publish(mqtt.payloadSetup_simple(dataTemplate,temp.temperature,humidity.relative_humidity,light));
    
    prev_time = millis(); // reset timer
    }

    mqtt.loop();
}