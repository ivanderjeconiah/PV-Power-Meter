#include <PZEM004Tv30.h> 
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#define MAX485_DE 16
#define MAX485_RE 5

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
        /* 1- PZEM-017 DC Energy Meter */
        if(millis() - startMillis1 > 5000)                                                                // Wait for 5 seconds as ESP Serial cause start up code crash
        {
          digitalWrite(MAX485_RE, 1);                                                                     /* put RE Pin to high*/
          digitalWrite(MAX485_DE, 1);                                                                     /* put DE Pin to high*/
          delay(1);                                                                                       // When both RE and DE Pin are high, converter is allow to transmit communication
        }
}

void postTransmission()                                                                                   /* Reception program when triggered*/
{
        
        /* 1- PZEM-017 DC Energy Meter */
        if(millis() - startMillis1 > 5000)                                                                // Wait for 5 seconds as ESP Serial cause start up code crash
        {
          delay(3);                                                                                       // When both RE and DE Pin are low, converter is allow to receive communication
          digitalWrite(MAX485_RE, 0);                                                                     /* put RE Pin to low*/
          digitalWrite(MAX485_DE, 0);                                                                     /* put DE Pin to low*/
        }
}

void setShunt(uint8_t slaveAddr)                                                                          //Change the slave address of a node
{

        /* 1- PZEM-017 DC Energy Meter */
        
        static uint8_t SlaveParameter = 0x06;                                                             /* Write command code to PZEM */
        static uint16_t registerAddress = 0x0003;                                                         /* change shunt register address command code */
        
        uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
        u16CRC = crc16_update(u16CRC, slaveAddr);                                                         // Calculate the crc16 over the 6bytes to be send
        u16CRC = crc16_update(u16CRC, SlaveParameter);
        u16CRC = crc16_update(u16CRC, highByte(registerAddress));
        u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
        u16CRC = crc16_update(u16CRC, highByte(NewshuntAddr));
        u16CRC = crc16_update(u16CRC, lowByte(NewshuntAddr));
      
        preTransmission();                                                                                /* trigger transmission mode*/
      
        PZEMSerial.write(slaveAddr);                                                                      /* these whole process code sequence refer to manual*/
        PZEMSerial.write(SlaveParameter);
        PZEMSerial.write(highByte(registerAddress));
        PZEMSerial.write(lowByte(registerAddress));
        PZEMSerial.write(highByte(NewshuntAddr));
        PZEMSerial.write(lowByte(NewshuntAddr));
        PZEMSerial.write(lowByte(u16CRC));
        PZEMSerial.write(highByte(u16CRC));
        delay(10);
        postTransmission();                                                                               /* trigger reception mode*/
        delay(100);
}

void changeAddress(uint8_t OldslaveAddr, uint8_t NewslaveAddr)                                            //Change the slave address of a node
{

        /* 1- PZEM-017 DC Energy Meter */
    
        static uint8_t SlaveParameter = 0x06;                                                             /* Write command code to PZEM */
        static uint16_t registerAddress = 0x0002;                                                         /* Modbus RTU device address command code */
        uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
        u16CRC = crc16_update(u16CRC, OldslaveAddr);                                                      // Calculate the crc16 over the 6bytes to be send
        u16CRC = crc16_update(u16CRC, SlaveParameter);
        u16CRC = crc16_update(u16CRC, highByte(registerAddress));
        u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
        u16CRC = crc16_update(u16CRC, highByte(NewslaveAddr));
        u16CRC = crc16_update(u16CRC, lowByte(NewslaveAddr));
        preTransmission();                                                                                 /* trigger transmission mode*/
        PZEMSerial.write(OldslaveAddr);                                                                       /* these whole process code sequence refer to manual*/
        PZEMSerial.write(SlaveParameter);
        PZEMSerial.write(highByte(registerAddress));
        PZEMSerial.write(lowByte(registerAddress));
        PZEMSerial.write(highByte(NewslaveAddr));
        PZEMSerial.write(lowByte(NewslaveAddr));
        PZEMSerial.write(lowByte(u16CRC));
        PZEMSerial.write(highByte(u16CRC));
        delay(10);
        postTransmission();                                                                                /* trigger reception mode*/
        delay(100);
}

void DC(){
  -kj
}

void setup(){
  Serial.begin(9600);
  PZEMDC.begin(9600,SWSERIAL_8N2,4,0);

  pinMode(MAX485_RE,OUTPUT);
  pinMode(MAX_485_DE,OUTPUT);
  digitalWrite(MAX485_RE,0);
  digitalWrite(MAX485_DE,0);

  node.preTransmission(preTransmission);
  node.poetTransmission(postTransmission);
  node.begin(pzemSlaveAddr,PZEMDC);
}

void loop (){
  AC();
}