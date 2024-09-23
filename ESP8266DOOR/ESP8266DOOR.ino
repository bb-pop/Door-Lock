#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "DSSI64_2.4G"; // Wi-Fi name
const char* password = "dssi64@ubu"; // Wi-Fi password
const char* mqtt_server = "192.168.0.100"; // IP RPI

WiFiClient espClient;
PubSubClient client(espClient);

const int pinLock = D1; // GPIO pin for the door lock control
const int ledWifi = LED_BUILTIN; // WiFi status LED
const int ledMQTT = D2; // MQTT status LED

unsigned long unlockTime = 0;
const unsigned long lockDelay = 5000; // Lock delay in milliseconds
bool isUnlocked = false;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(ledWifi, !digitalRead(ledWifi)); // Blink LED while connecting
  }
  digitalWrite(ledWifi, HIGH); // Turn on LED when connected
}

void callback(String topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (topic == "SC252/doorLock" && messageTemp == "unlock" && !isUnlocked) {
    digitalWrite(pinLock, LOW); // Unlock (LOW signal)
    unlockTime = millis();
    isUnlocked = true;
    digitalWrite(ledMQTT, HIGH); // Turn on MQTT LED on unlock
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("SC252")) {
      client.subscribe("SC252/doorLock");
    } else {
      delay(5000);
    }
  }
  digitalWrite(ledMQTT, LOW); // Turn off MQTT LED when disconnected
}

void setup() {
  pinMode(pinLock, OUTPUT);
  pinMode(ledWifi, OUTPUT);
  pinMode(ledMQTT, OUTPUT);

  digitalWrite(pinLock, HIGH); // Initially locked (HIGH signal)
  digitalWrite(ledWifi, LOW);
  digitalWrite(ledMQTT, LOW);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (isUnlocked && millis() - unlockTime > lockDelay) {
    digitalWrite(pinLock, HIGH); // Re-lock (HIGH signal)
    isUnlocked = false;
    digitalWrite(ledMQTT, LOW); // Turn off MQTT LED on lock
  }
}