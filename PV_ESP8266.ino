#include <PZEM004Tv30.h> 
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#define MAX485_DE 16
#define MAX485_RE 0

PZEM004Tv30 pzem(13,15);
ModbusMaster node;
SoftwareSerial PZEMDC;

//var AC sensor
float ACVoltage, ACCurrent,ACPower,ACFrequency,ACEnergy,cosPhi;

//var for DC
static uint8_t pzemSlaveAddr =0x01;
static uint16_t NewShuntAddr = 0x0000;
float DCVoltage,DCCurrent,DCPower,DCEnergy;

void AC(){
  ACVoltage = pzem.voltage();
  ACCurrent = pzem.current();
  ACPower= pzem.power();
  ACFrequency= pzem.frequency();
  ACEnergy= pzem.energy();
  cosPhi= pzem.pf();

  Serial.println("AC V = " + String(ACVoltage));
  Serial.println("AC A = " + String(ACCurrent));
  Serial.println("AC P = " + String(ACPower));
  Serial.println("AC E = " + String(ACEnergy));
  Serial.println("AC F = " + String(ACFrequency));
  Serial.println("AC PF = " + String(cosPhi));
}

void preTransmission()                                                                                    /* transmission program when triggered*/
{
          digitalWrite(MAX485_RE, 1);                                                                     /* put RE Pin to high*/
          digitalWrite(MAX485_DE, 1);                                                                     /* put DE Pin to high*/
          delay(1);                                                                                       // When both RE and DE Pin are high, converter is allow to transmit communication
}

void postTransmission()                                                                                   /* Reception program when triggered*/
{
          delay(3);                                                                                       // When both RE and DE Pin are low, converter is allow to receive communication
          digitalWrite(MAX485_RE, 0);                                                                     /* put RE Pin to low*/
          digitalWrite(MAX485_DE, 0);                                                                     /* put DE Pin to low*/
}

void DC(){
  uint8_t result;
  result = node.readInputRegisters(0x0000,6);
  if (result == node.ku8MBSuccess){
    uint32_t tempDouble= 0x00000000;
    DCVoltage = node.getResponseBuffer(0x0000) / 100.0;
    DCCurrent = node.getResponseBuffer(0x0001) / 100.0;

    tempDouble = (node.getResponseBuffer(0x0003) << 16) + node.getResponseBuffer(0x0002);
    DCPower = tempDouble / 10.0;

    tempDouble = (node.getResponseBuffer(0x0005) <<16) + node.getResponseBuffer(0x0004);
    DCEnergy = tempDouble;
  }

  Serial.println("DC V = " + String(DCVoltage));
  Serial.println("DC A = " + String(DCCurrent));
  Serial.println("DC P = " + String(DCPower));
  Serial.println("DC E = " + String(DCEnergy));
  
}

void setup(){
  Serial.begin(9600);
  PZEMDC.begin(9600,SWSERIAL_8N2,2,14);

  pinMode(MAX485_RE,OUTPUT);
  pinMode(MAX485_DE,OUTPUT);
  digitalWrite(MAX485_RE,0);
  digitalWrite(MAX485_DE,0);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.begin(pzemSlaveAddr,PZEMDC);
}

void loop (){
  AC();
  DC();
}
