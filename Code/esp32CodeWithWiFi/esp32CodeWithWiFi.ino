#include <WiFi.h>
#include <Adafruit_AHTX0.h>
#include <PubSubClient.h>
char mac[17];
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "142.93.174.193";
const char* topic1 = "zuerich/briefkasten/ultraschall/in";
const int trigPin = 18;
const int echoPin = 5;
#define SOUND_SPEED 0.034
long duration;
float distanceCm;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("done!");
  byte m[6];
  WiFi.macAddress(m);
  sprintf(mac, "%02X:02X:02X:02X:02X:02X", m[0], m[1], m[2], m[3], m[4], m[5]);
}

/*
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, topic1) == 0) {
    char message[length];
    for (int i = 0; i < length; i++) {
      message[i] = (char)payload[i];
    }
    //Serial.print("Received Message: ");
    //Serial.println(message);
  }
}
*/

void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while (!client.connected()) {
    if (client.connect(mac)) {
      Serial.println("done!");
      client.subscribe(topic1);
    } else {
      delay(500);
      Serial.print(".");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  char distanceCM[20];
  sprintf(distanceCM, "%f", distanceCm);
  client.publish(topic1, distanceCM);

  delay(1000);
  client.loop();
}