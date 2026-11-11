#include <WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = "VOTRE_WIFI";
const char* WIFI_MDP  = "VOTRE_MOT_DE_PASSE";
const char* BROKER    = "broker.hivemq.com";
const int   PORT      = 1883;

// Choisissez un préfixe unique : le broker public est partagé par tout le monde
const char* TOPIC_MESURE = "monprojet123/capteur/valeur";
const char* TOPIC_LED    = "monprojet123/led/commande";

const int PIN_CAPTEUR = 34;
const int PIN_LED = 2;

WiFiClient wifi;
PubSubClient mqtt(wifi);
unsigned long dernierEnvoi = 0;

void recevoir(char* topic, byte* payload, unsigned int longueur) {
  String msg;
  for (unsigned int i = 0; i < longueur; i++) msg += (char)payload[i];
  if (msg == "ON")  digitalWrite(PIN_LED, HIGH);
  if (msg == "OFF") digitalWrite(PIN_LED, LOW);
}

void connecter() {
  while (!mqtt.connected()) {
    String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (mqtt.connect(id.c_str())) {
      mqtt.subscribe(TOPIC_LED);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_MDP);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(recevoir);
}

void loop() {
  if (!mqtt.connected()) connecter();
  mqtt.loop();

  if (millis() - dernierEnvoi > 2000) {
    dernierEnvoi = millis();
    int valeur = analogRead(PIN_CAPTEUR);   // 0 à 4095 sur ESP32
    mqtt.publish(TOPIC_MESURE, String(valeur).c_str());
  }
}