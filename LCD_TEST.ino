//#include <PZEM004Tv30.h> 
//#include <SoftwareSerial.h>
//#include <ModbusMaster.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//#define MAX485_DE 16
//#define MAX485_RE 0

//PZEM004Tv30 pzem(13,15);
//ModbusMaster node;
//SoftwareSerial PZEMDC;
LiquidCrystal_I2C lcd(0x27,20,4);

//var AC sensor
float ACVoltage, ACCurrent,ACPower,ACFrequency,ACEnergy,cosPhi;

//var for DC
static uint8_t pzemSlaveAddr =0x01;
static uint16_t NewShuntAddr = 0x0000;
float DCVoltage,DCCurrent,DCPower,DCEnergy;

//var for reading
long int timer;

//var for page control
uint8_t page=1;

//void AC(){
//  ACVoltage = pzem.voltage();
//  ACCurrent = pzem.current();
//  ACPower= pzem.power();
//  ACFrequency= pzem.frequency();
//  ACEnergy= pzem.energy();
//  cosPhi= pzem.pf();
//
//  Serial.println("AC V = " + String(ACVoltage));
//  Serial.println("AC A = " + String(ACCurrent));
//  Serial.println("AC P = " + String(ACPower));
//  Serial.println("AC E = " + String(ACEnergy));
//  Serial.println("AC F = " + String(ACFrequency));
//  Serial.println("AC PF = " + String(cosPhi));
//}
//
//void preTransmission()                                                                                    /* transmission program when triggered*/
//{
//          digitalWrite(MAX485_RE, 1);                                                                     /* put RE Pin to high*/
//          digitalWrite(MAX485_DE, 1);                                                                     /* put DE Pin to high*/
//          delay(1);                                                                                       // When both RE and DE Pin are high, converter is allow to transmit communication
//}
//
//void postTransmission()                                                                                   /* Reception program when triggered*/
//{
//          delay(3);                                                                                       // When both RE and DE Pin are low, converter is allow to receive communication
//          digitalWrite(MAX485_RE, 0);                                                                     /* put RE Pin to low*/
//          digitalWrite(MAX485_DE, 0);                                                                     /* put DE Pin to low*/
//}
//
//void DC(){
//  uint8_t result;
//  result = node.readInputRegisters(0x0000,6);
//  if (result == node.ku8MBSuccess){
//    uint32_t tempDouble= 0x00000000;
//    DCVoltage = node.getResponseBuffer(0x0000) / 100.0;
//    DCCurrent = node.getResponseBuffer(0x0001) / 100.0;
//
//    tempDouble = (node.getResponseBuffer(0x0003) << 16) + node.getResponseBuffer(0x0002);
//    DCPower = tempDouble / 10.0;
//
//    tempDouble = (node.getResponseBuffer(0x0005) <<16) + node.getResponseBuffer(0x0004);
//    DCEnergy = tempDouble;
//  }
//
//  Serial.println("DC V = " + String(DCVoltage));
//  Serial.println("DC A = " + String(DCCurrent));
//  Serial.println("DC P = " + String(DCPower));
//  Serial.println("DC E = " + String(DCEnergy));
//  
//}

void Halaman1(){
  lcd.setCursor(4,0);
  lcd.print("AC MONITORING");

  lcd.setCursor(0,1);
  lcd.print("V:     V FREQ:    Hz");


  lcd.setCursor(0,2);
  lcd.print("I:     A COS :");

  lcd.setCursor(0,3);
  lcd.print("P:     W T: 14:15:07");
  
}

void Halaman2(){
  lcd.setCursor(0,0);
  lcd.print("V:     V  Psh:     H");

  lcd.setCursor(0,1);
  lcd.print("I:     A  PV :     %");

  lcd.setCursor(0,2);
  lcd.print("P:     W  SOC:     %");
  
  lcd.setCursor(0,3);
  lcd.print("D:    kWh Y :    KWh");
  
}

void Halaman4(){
  lcd.setCursor(0,0);
  lcd.print("RECORD KWh 1 MINGGU");
  lcd.setCursor(0,1);
  lcd.print("D1:       D4:");
  lcd.setCursor(0,2);
  lcd.print("D1:       D4:");
  lcd.setCursor(0,3);
  lcd.print("D1:       D4:");
  
}

void Halaman3(){
  lcd.setCursor(0,0);
  lcd.print("AC P TOT:        KWh");
  lcd.setCursor(0,1);
  lcd.print("DC P TOT:        KWh");
  lcd.setCursor(0,2);
  lcd.print("RUPIAH:");
  lcd.setCursor(0,3);
  lcd.print("STATUS: DISCHARGING");
  
}

//void updateData(){
//  
//}

void setup(){
  Serial.begin(9600);
  //PZEMDC.begin(9600,SWSERIAL_8N2,2,14);

  //pinMode(MAX485_RE,OUTPUT);
  //pinMode(MAX485_DE,OUTPUT);
  //digitalWrite(MAX485_RE,0);
  //digitalWrite(MAX485_DE,0);

  //node.preTransmission(preTransmission);
  //node.postTransmission(postTransmission);
  //node.begin(pzemSlaveAddr,PZEMDC);

  pinMode(12,INPUT_PULLUP);
  Wire.begin(D2,D1);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(100);
  Halaman1();
  timer=millis();
}

void loop (){
  if(digitalRead(12)==LOW){
    delay(10);
    if(digitalRead(12)==LOW){
      Serial.println("NEXT PAGE");
      page++;
      if(page>4){
        page=1;
      }
      lcd.clear();
      if(page==1){
        Halaman1();
      }
      else if(page==2){
        Halaman2();
      }
      else if(page==3){
        Halaman3();
      }
      else if(page==4){
        Halaman4();
      }
    }
  }
//  if(millis()-timer>=1000){
//    AC();
//    DC();
//  }
}
