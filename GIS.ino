//Buka Tools---Board Manager (1. Board=Arahkan Ke Wemos D1 R1. 2.Upload Speed 115200. 3. Port COM )




#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <DHT_U.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN 2     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


Adafruit_MLX90614 mlx = Adafruit_MLX90614();


//Penggantian WIFI
const char* ssid = "TP-Link_99B4";  //Nama WIFI Harus Sesuai
const char* password = "16622180"; //Password Wifi

//Program yang berada dibawah mohon tidak dirubah



const char* mqtt_server = "202.154.58.84";
const char* mqtt_user = "arifin";
const char* mqtt_password = "arifinjuga";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String payloadtemp;

  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
   payloadtemp += (char)payload[i];
  }
  Serial.println();

  if(topic=="arifin/lampu1"){
    Serial.print(payloadtemp);
  }else if(topic=="arifin/lampu2"){
    Serial.println(payloadtemp);
  }
}

void reconnect(){
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)){
      Serial.println("connected");
      client.subscribe("arifin/lampu1");
      client.subscribe("arifin/lampu2");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();  
  Serial.println(F("DHTxx test!"));
  dht.begin();
 
}

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); 
  Serial.print("*C\tKelembapan = "); Serial.print(dht.readHumidity()); Serial.println("*C");
 
  float a = mlx.readAmbientTempC();
  float o =mlx.readObjectTempC();
  float h = dht.readHumidity();

  char ambient[7];
  dtostrf(a,0,2,ambient);

  char object[7];
  dtostrf(o,0,0,object);

  char humidity[7];
  dtostrf(h,0,0,humidity);

  client.publish("arifin/suhu", ambient);
  client.publish("arifin/suhuobject", object);
  client.publish("arifin/humidity", humidity);
  
  
  Serial.println();
  delay(500);
}
