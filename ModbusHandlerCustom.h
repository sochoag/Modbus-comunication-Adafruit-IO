#ifndef ModbusHandlerCustom
#define ModbusHandlerCustom

#include <ModbusMaster.h>

#define MAX485_DE_RE   5
#define SLAVE_ID 1
#define RX_PIN 16
#define TX_PIN 17

ModbusMaster modbus;

bool status1 = false, lastStatus1 = true;
bool status2 = false, lastStatus2 = true;
unsigned long last = 0;

float fV1;
float fV2;
float fV3;
float fC;
bool banderaModbus = false;

void preTransmission()
{
  digitalWrite(MAX485_DE_RE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_DE_RE, 0);
}

void ModbusSetup()
{
  Serial.println("Inicializando Modbus");

  pinMode(MAX485_DE_RE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_DE_RE, 0);

  // Modbus communication runs at 115200 baud
  Serial1.begin(9600, SERIAL_8N2, RX_PIN, TX_PIN);

  // Modbus slave ID 1
  modbus.begin(SLAVE_ID, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
  modbus.writeSingleCoil(0x462E, false);
  modbus.writeSingleCoil(0x462E, false);

  Serial.println("Modbus Inicializado");
}


void ModbusLoop()
{
  uint8_t result;
  uint16_t data[6];

  if(status1 != lastStatus1)
  {
    lastStatus1 = status1;
    String rele1 = status1 ? "Activado":"Desactivado";
    result = modbus.writeSingleCoil(0x462E, status1);
    Serial.println("Estado rele1:" + rele1); 
  }

  if(status2 != lastStatus2)
  {
    lastStatus2 = status2;
    String rele2 = status2 ? "Activado":"Desactivado";
    result = modbus.writeSingleCoil(0x462F, status2);
    Serial.println("Estado rele2:" + rele2);
  }  

  if(millis()-last > 10000)
  {
    last = millis();
    result = modbus.readHoldingRegisters(0x0008,8);
    if (result == modbus.ku8MBSuccess)
    {
      //Serial.println("---------MEDICIONES---------");
      //Serial.print("VL1: ");
      unsigned long vl1 = (modbus.getResponseBuffer(0x01)+(modbus.getResponseBuffer(0x00)<<16));
      //Serial.println(*(float*) &vl1);
      //Serial.print("VL2: ");
      unsigned long vl2 = (modbus.getResponseBuffer(0x03)+(modbus.getResponseBuffer(0x02)<<16));
      //Serial.println(*(float*) &vl2);
      //Serial.print("VL3: ");
      unsigned long vl3 = (modbus.getResponseBuffer(0x05)+(modbus.getResponseBuffer(0x04)<<16));
      //Serial.println(*(float*) &vl3);
      //Serial.print("Current: ");
      unsigned long c = (modbus.getResponseBuffer(0x07)+(modbus.getResponseBuffer(0x06)<<16));
      //Serial.println(*(float*) &c);
      fV1 = *(float*) &vl1;
      fV2 = *(float*) &vl2;
      fV3 = *(float*) &vl3;
      fC = *(float*) &c;
      banderaModbus = true;
    }
  }
}
#endif
