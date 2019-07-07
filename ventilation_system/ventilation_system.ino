#include <PubSubClient.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <dht.h>


dht DHT;
int ledPin = 13;

#define DHT11_PIN 7
#define ORG "******"
#define DEVICE_TYPE "SVSysh"
#define DEVICE_ID "hexpra"
#define TOKEN "**********"
#define WIFI_AP "Red"
#define WIFI_PASSWORD "*********"
         
WiFiEspClient espClient;

SoftwareSerial soft(10,11); // 10-tx,11-rx
int status = WL_IDLE_STATUS;

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char publishTopic[] = "iot-2/evt/status/fmt/json";
const char responseTopic[] = "iotdm-1/response";
const char manageTopic[] = "iotdevice-1/mgmt/manage";
const char updateTopic[] = "iotdm-1/device/update";
const char rebootTopic[] = "iotdm-1/mgmt/initiate/device/reboot";

void callback(char* publishTopic, char* payload, unsigned int payloadLength);

PubSubClient client(server, 1883, callback, espClient);
long lastPublishMillis;
int period = 5000;
unsigned long time_now = 0;
int rate=0;            
void setup() {
        Serial.begin(115200);
        InitWiFi();
        pinMode(ledPin, OUTPUT);
        Serial.print(WiFi.localIP());
        if (!!!client.connected()) {
                Serial.print("Reconnecting client to ");
                Serial.println(server);
                while (!!!client.connect(clientId, authMethod, token)) {
                Serial.print(".");
        }
        Serial.println();
        }

}


void loop() {
          int chk = DHT.read11(DHT11_PIN);
          int trig=0;
          double temp,humi,gas;
          Serial.print("Temperature = ");
          temp=DHT.temperature;
          Serial.println(temp);
          Serial.print("Humidity = ");
          humi=DHT.humidity;
          Serial.println(humi);
          gas = analogRead(A0);  
          Serial.print("Airquality = ");
          Serial.println(gas);   
          if(temp >=35 || humi >= 25 ||  gas >= 250){
              digitalWrite(ledPin, HIGH);
              trig=1;
          }
          else {
              digitalWrite(ledPin, LOW);
              trig=0;
          }
          String payload = "{\"d\":{ \"Temperature\" :";
          payload += temp;
          payload += " , \"Humidity\" :";
          payload += humi;
          payload += " , \"Gas\" :";
          payload += gas;
          payload += " , \"Alert\" :";
          payload += trig;
          payload += "} }";
          Serial.print("Sending payload: ");
          Serial.println(payload);

          while (!!!client.publish(publishTopic, (char *)payload.c_str())) {
                  Serial.println("Publish ok");
                  if (!!!client.connected()) {
                        Serial.print("Reconnecting client to ");
                        Serial.println(server);
                        while (!!!client.connect(clientId, authMethod, token)) {
                              Serial.print(".");
                              delay(1000);
                        }
                        Serial.println();
                  }
          
          } 
            time_now = millis();           
            Serial.println("5 sec Delay");           
            while(millis() < time_now + period){
                //wait approx. [period] ms
            }  
}

void InitWiFi()
{
      // initialize serial for ESP module
      soft.begin(112500);
      // initialize ESP module
      WiFi.init(&soft);
      
      Serial.println("Connecting to AP …");
      // attempt to connect to WiFi network
      while ( status != WL_CONNECTED) {
            Serial.print("Attempting to connect to WPA SSID: ");
            Serial.println(WIFI_AP);
            // Connect to WPA/WPA2 network
            status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
            delay(1000);
      }
      Serial.println("Connected to AP");
}

void callback(char* publishTopic, char* payload, unsigned int length) {
      Serial.println("callback invoked");
}
