#include<SoftwareSerial.h>

SoftwareSerial ESPCon(9,8);

#define R1 30000.0
#define R2 7500.0
#define VoltageSensor1 A0
#define VoltageSensor2 A1
#define CurrentSensor A2
#define LDR A3

//var for voltage
float adcVoltage=0, inVoltage1=0, inVoltage2=0;
int adcValue=0;

//var for current
double Vout=0,Current=0;
//const double scale_factor = 0.185; // 5A
const double scale_factor = 0.1; // 20A
//const double scale_factor = 0.066; // 30A

const double vRef = 5.00;
const double resConvert = 1024;
double resADC = vRef/resConvert;
double zeroPoint = vRef/2;

//var for sending data
String data;
byte buf[100];

//var for PV Power
double pvPow=0;

//var for LDR
const float gamma=0.7;
const float RL10=50;
float lux;

void readVoltage1(){
  adcValue = analogRead(VoltageSensor1);
  adcVoltage  = (adcValue * 5.0) / 1024.0; 
  inVoltage1 = adcVoltage / (R2/(R1+R2)) ; 
  Serial.print("First Voltage Sensor = ");
  Serial.println(inVoltage1, 2);
}

void readVoltage2(){
  adcValue = analogRead(VoltageSensor2);
  adcVoltage  = (adcValue * 5.0) / 1024.0; 
  inVoltage2 = adcVoltage / (R2/(R1+R2)) ; 
  Serial.print("Second Voltage Sensor = ");
  Serial.println(inVoltage2, 2);
}

void readCurrent(){
  for(int i=0; i < 1000 ;i++){
    Vout=(Vout+(resADC*analogRead(A0)));
    delay(1);
  }
  Vout=Vout/1000;
  Current=(Vout-zeroPoint)/scale_factor;
  Serial.print("Current Sensor = ");
  Serial.println(Current, 2);
}

void sendDat(){
  data=String(inVoltage1)+" "+String(Current)+" "+String(pvPow)+" "+String(inVoltage2)+" "+String(lux)+"=";
  Serial.println(data);
  data.getBytes(buf,data.length()+1);
  ESPCon.write(buf, data.length());
}

void readLux(){
  int analogValue=analogRead(LDR);
  float voltage= analogValue *5.0/1023.0;
  float resistance= 2000 *voltage /(1-voltage/5);
  lux = pow(RL10 *1e3 * pow(10,gamma)/resistance, (1/gamma));
  Serial.print("Light Intensity = ");
  Serial.println(lux);
}

void setup(){
  Serial.begin(9600);
}

void loop(){
  readVoltage1();
  readCurrent();
  pvPow= inVoltage1*Current;
  readVoltage2();
  readLux();
  sendDat();
  delay(1000);
}
