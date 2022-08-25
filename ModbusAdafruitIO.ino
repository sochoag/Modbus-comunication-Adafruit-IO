#include "WifiManagerCustom.h"
#include "ModbusHandlerCustom.h"
#include "MQTTHandlerCustom.h"

void setup() 
{
  Serial.begin(115200);
  Serial.println("Monitor IoT - Adafruit IO");
  delay(500);
  WiFisetup();
  if(bandera)
  {
    MQTTSetup();
    ModbusSetup();
  }
}

void loop() 
{
  if(bandera)
  {
    ModbusLoop();
    MQTTLoop();
    if(banderaModbus)
    {
      Serial.println("Mensajes publicados");
      banderaModbus = false;
      client.publish("Voltage1Feed", String(fV1).c_str());
      client.publish("Voltage2Feed", String(fV2).c_str());
      client.publish("Voltage3Feed", String(fV3).c_str());
      client.publish("CurrentFeed", String(fC).c_str());
    }
  }
  else
  {
    Serial.println("Esperando conexión wifi");
    delay(10000);
  }
}
