//#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID           "TMPxxxxxx"
#define BLYNK_TEMPLATE_NAME         "Device"
#define BLYNK_AUTH_TOKEN            "YourAuthToken"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//variable declaration
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";
String receivedData,bantu;
int j;
float data[8];


void setup(){
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}


void loop(){
  Blynk.run();
  if (Serial.available()){
    receivedData= Serial.readStringUntil('=');
    j=0;
    bantu="";
    for (int i=0; i< receivedData.length(); i++){
      if(receivedData[i] == ' '){
        data[j]=  bantu.toFloat();
        bantu="";
        j++;
      }
      else{
        if((isDigit(receivedData[i])) || (receivedData[i]=='.')){
          bantu=bantu+receivedData[i];
        }
        else{
          bantu="";
        }
      }
    }
    data[j]= bantu.toFloat();
  }

  Blynk.virtualWrite(V0,data[0]);
  Blynk.virtualWrite(V1,data[1]);
  Blynk.virtualWrite(V2,data[2]);
  Blynk.virtualWrite(V3,data[3]);
  Blynk.virtualWrite(V4,data[4]);
  Blynk.virtualWrite(V5,data[5]);
  Blynk.virtualWrite(V6,data[6]);
  Blynk.virtualWrite(V7,data[7]);
}
