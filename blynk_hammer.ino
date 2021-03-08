#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "secret.h"

#define MIC_PIN 0
#define NUM_OF_STRIPS 2

WiFiUDP Udp;
WidgetBridge lightningBridge(V1);
BlynkTimer thunder;
int mode_value = 0;

IPAddress strip_ips[NUM_OF_STRIPS] = { IPAddress(192,168,254,26), IPAddress(192,168,254,27) };

struct thunder_data {
  uint32_t data;
};

BLYNK_WRITE(V2)
{
    mode_value = param.asInt();
    lightningBridge.virtualWrite(V2, mode_value);
}

BLYNK_WRITE(V3)
{
  lightningBridge.virtualWrite(V3, param.asInt());
}

BLYNK_WRITE(V4)
{
  lightningBridge.virtualWrite(V4, param.asInt());
}

BLYNK_WRITE(V5)
{
  lightningBridge.virtualWrite(V5, param.asInt());
}

BLYNK_WRITE(V6)
{
  lightningBridge.virtualWrite(V6, param.asInt());
}

BLYNK_CONNECTED()
{
    lightningBridge.setAuthToken(light_auth);
}


void setup() {
    pinMode(MIC_PIN, INPUT);
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    while (Blynk.connect() == false) {
      //Blynk.connect()
    }
    Udp.begin(7777);
    Serial.println(WiFi.localIP());
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    thunder.setInterval(20L, myThunderEvent);
}

void loop() {
    Blynk.run();
    thunder.run();
}

void myThunderEvent()
{
  if(!mode_value) {//Music
    uint32_t analogRaw = analogRead(MIC_PIN);
    if (analogRaw < 3) {
        return;
    }
    sendThunder(analogRaw);
  }
}

void sendThunder(uint32_t analogRaw) {
  struct thunder_data sendThunderData;
  sendThunderData.data = analogRaw; 
  for (int i = 0; i < NUM_OF_STRIPS; i++) {
    Udp.beginPacket(strip_ips[i], 8888);
    Udp.write((char *)&sendThunderData, sizeof(struct thunder_data));
    Udp.endPacket();
  }
}
