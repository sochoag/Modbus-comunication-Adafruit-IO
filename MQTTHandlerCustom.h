#ifndef MQTTHandlerCustom
#define MQTTHandlerCustom

#include <PubSubClient.h>
#include "WifiManagerCustom.h"

const char* mqtt_server = "io.adafruit.com";

PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length)
{
  if(strcmp(topic,"ReleFeed1")==0)
  {
    status1 = (char)payload[0] == '1'?true:false;
  }
  if(strcmp(topic,"ReleFeed2")==0)
  {
    status2 = (char)payload[0] == '1'?true:false;
  }
}

void reconnect()
{
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), "YOUR-USER", "YOUR-API-KEY"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      client.publish("ReleFeed1","0");
      client.publish("ReleFeed2","0");
      // ... and resubscribe
      client.subscribe("ReleFeed1");
      client.subscribe("ReleFeed2");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void MQTTSetup()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void MQTTLoop()
{
  if(!client.connected())
  {
    reconnect();
  }
  client.loop();
}

#endif