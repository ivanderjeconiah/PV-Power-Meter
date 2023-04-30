//#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6bL1P67s7"
#define BLYNK_TEMPLATE_NAME "PVMonitoring"
#define BLYNK_AUTH_TOKEN "ur3y7gAE_htbTCLpu62OTBOvR0IOPcXi"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//variable declaration
char ssid[] = "Steven";
char pass[] = "S14ntur1";
String receivedData, bantu;
int j;
float data[9];


void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}


void loop() {
  Blynk.run();
  if (Serial.available()) {
    receivedData = Serial.readStringUntil('=');
    j = 0;
    bantu = "";
    for (int i = 0; i < receivedData.length(); i++) {
      if (receivedData[i] == ' ') {
        data[j] =  bantu.toFloat();
        bantu = "";
        j++;
      }
      else {
        if ((isDigit(receivedData[i])) || (receivedData[i] == '.')) {
          bantu = bantu + receivedData[i];
        }
        else {
          bantu = "";
        }
      }
    }
    data[j] = bantu.toFloat();
    Blynk.virtualWrite(V0, data[0]);
    Blynk.virtualWrite(V1, data[1]);
    Blynk.virtualWrite(V2, data[2]);
    Blynk.virtualWrite(V3, data[3]);
    Blynk.virtualWrite(V4, data[4]);
    Blynk.virtualWrite(V5, data[5]);
    Blynk.virtualWrite(V6, data[6]);
    Blynk.virtualWrite(V7, data[7]);
    Blynk.virtualWrite(V8, data[8]);
  }
  delay(50);
}

BLYNK_WRITE(V9){
  bool hasil = param.asInt();
  if(hasil){
     Serial.write("1=");
  }
}
