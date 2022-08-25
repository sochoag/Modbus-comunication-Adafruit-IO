#ifndef WIFIMANAGERCUSTOM
#define WIFIMANAGERCUSTOM

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"

WiFiClient espClient;
// SOFT AP
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,4);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
bool bandera = false;

String ssid;
String pass;

const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

IPAddress localIP;

unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Initialize SPIFFS
void initSPIFFS() 
{
  if (!SPIFFS.begin(true)) 
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;

  while(file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }

  if(file.print(message))
  {
    Serial.println("- file written");
  } 
  else 
  {
    Serial.println("- frite failed");
  }
}

// Initialize WiFi
bool initWiFi() 
{
  if(ssid=="")
  {
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) 
  {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) 
    {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  bandera = true;
  return true;
}


bool WiFisetup() 
{
  initSPIFFS();

  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);

  Serial.println(ssid);
  Serial.println(pass);

  if(!initWiFi()) 
  {
    // Connect to Wi-Fi network with SSID and password
    Serial.print("Setting soft-AP configuration ... ");
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP("Monitor IoT", NULL);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) 
    {
      int params = request->params();
      for(int i=0;i<params;i++)
      {
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost())
        {
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) 
          {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) 
          {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart");
      delay(3000);
      ESP.restart();
    });
    server.begin();
  }
  else
  {
      return false;
  }
}

#endif
