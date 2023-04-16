#include <PZEM004Tv30.h> 
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MAX485_DE 16
#define MAX485_RE 0
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLiy9Icizd"
#define BLYNK_TEMPLATE_NAME "Power Meter"
#define BLYNK_AUTH_TOKEN "H69ULqGnrCSkdx1wbz303k8IM4mABVOW"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

PZEM004Tv30 pzem(13,12);
ModbusMaster node;
SoftwareSerial PZEMDC;
LiquidCrystal_I2C lcd(0x27, 20,4);

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

//var of clock
String jam;
long int clockTimer;
bool getTime=false;
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;  
const int daylightOffset_sec = 0;

//var for BLYNK
char ssid[]= "s";
char pass[]= "11111111";

//var for power
double WeekDataAC[6];

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

  //update LCD's value
  for (int i=2; i<=6 ; i++) {
      lcd.setCursor(i,1);
      lcd.print(" ");
      lcd.setCursor(i,2);
      lcd.print(" ");
      lcd.setCursor(i,3);
      lcd.print(" ");
  }
  lcd.setCursor(2,1);
  lcd.print(String(ACVoltage,2));
  uint8_t totalString = String(ACCurrent,0).length();
  lcd.setCursor(2,2);
  lcd.print(String(ACCurrent,3+((-1)*(totalString-1))));
  lcd.setCursor(2,3);
  totalString = String(ACPower,0).length();
  lcd.print(String(ACPower, 3+((-1)*(totalString-1))));

  for (int i=14; i<=17;i++){
    lcd.setCursor(i,1);
    lcd.print(" ");
    lcd.setCursor(i,2);
    lcd.print(" ");
  }

  lcd.setCursor(14,1);
  lcd.print(String(ACFrequency,1));
  lcd.setCursor(14,2);
  lcd.print(String(cosPhi,1));

  for(int i=9;i<=16; i++){
    lcd.setCursor(i,1);
    lcd.print(" ");
  }
  lcd.setCursor(9,1);
  lcd.print(String(ACEnergy,2));
}

void preTransmission(){
          digitalWrite(MAX485_RE, 1);                                                                    
          digitalWrite(MAX485_DE, 1);                                                                    
          delay(1);               
}

void postTransmission(){                                                                             
          delay(3);                                                                                       
          digitalWrite(MAX485_RE, 0);
          digitalWrite(MAX485_DE, 0);                                                                     
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

  for (int i=2; i<=6 ; i++) {
      lcd.setCursor(i,1);
      lcd.print(" ");
      lcd.setCursor(i,2);
      lcd.print(" ");
      lcd.setCursor(i,3);
      lcd.print(" ");
  }
  lcd.setCursor(2,1);
  lcd.print(String(DCVoltage,2));
  uint8_t totalString = String(DCCurrent,0).length();
  lcd.setCursor(2,2);
  lcd.print(String(DCCurrent,3+((-1)*(totalString-1))));
  lcd.setCursor(2,3);
  totalString = String(DCPower,0).length();
  lcd.print(String(DCPower, 3+((-1)*(totalString-1)))); 

  for(int i=9;i<=16; i++){
    lcd.setCursor(i,2);
    lcd.print(" ");
  }
  lcd.setCursor(9,2);
  lcd.print(String(DCEnergy,2));
}

void Halaman1(){
  lcd.setCursor(4,0);
  lcd.print("AC MONITORING");

  lcd.setCursor(0,1);
  lcd.print("V:     V FREQ:    Hz");


  lcd.setCursor(0,2);
  lcd.print("I:     A COS :");

  lcd.setCursor(0,3);
  lcd.print("P:     W T:");
  
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
  lcd.setCursor(1,0);
  lcd.print("ENERGY CALCACULATION");
  lcd.setCursor(0,1);
  lcd.print("AC P TOT:        KWh");
  lcd.setCursor(0,2);
  lcd.print("DC P TOT:        KWh");
  lcd.setCursor(0,3);
  lcd.print("STATUS: DISCHARGING");
  
}

void LocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    if (timeinfo.tm_hour < 10) {
      jam = "0" + String(timeinfo.tm_hour) + ":";
    } else {
      jam = String(timeinfo.tm_hour) + ":";
    }
    if (timeinfo.tm_min < 10) {
      jam += "0" + String(timeinfo.tm_min) + ":";
    } else {
      jam += String(timeinfo.tm_min) + ":";
    }
    if (timeinfo.tm_sec < 10) {
      jam += "0" + String(timeinfo.tm_sec);
    } else {
      jam += String(timeinfo.tm_sec);
    }
  }
}

void setup(){
  Serial.begin(9600);
  PZEMDC.begin(9600,SWSERIAL_8N2,2,14);

  //Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass);

  WiFi.begin(ssid,pass);
  pinMode(MAX485_RE,OUTPUT);
  pinMode(MAX485_DE,OUTPUT);
  digitalWrite(MAX485_RE,0);
  digitalWrite(MAX485_DE,0);

  //take time
  while (!getTime) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
    } 
    else {
      //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      jam = String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec);
      Serial.println(jam);
      getTime = 1;
    }
    delay(500);
  }

  //connect blynk
  Blynk.config(BLYNK_AUTH_TOKEN);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.begin(pzemSlaveAddr,PZEMDC);

  pinMode(15,INPUT);

  Wire.begin(D2,D1);
  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  delay(100);
  Halaman1();
  
  timer=millis();
  clockTimer=millis();
}

void loop (){
  Blynk.run();
  if(digitalRead(15)==HIGH){
    delay(130);
    if(digitalRead(15)==HIGH){
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
  if(millis()-timer>=1500){
    AC();
    DC();
    timer=millis();
  }
  if(millis()-clockTimer>=1000){
    LocalTime();

    //if(timeinfo.tm_hour==5) && (timeinfo.tm_min>=0){
      //reset all sensor
    //}

    //update time at lcd
    for (int i=12; i<=19; i++){
      lcd.setCursor(i,3);
      lcd.print(" ");
    }
    lcd.setCursor(12,3);
    lcd.print(jam);
    
    clockTimer=millis();
  } 
}
