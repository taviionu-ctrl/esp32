#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "HOTSPOT_NAME";
const char* password = "HOTSPOT_PASS";

// DUPĂ deploy pe Render: ex. "https://numele-tau.onrender.com"
String API_BASE = "https://YOUR-RENDER-APP.onrender.com";

const int LED_PIN = 4; // GPIO4 (LED-ul tău)

unsigned long lastPoll = 0;
const unsigned long POLL_MS = 1500;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 local IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastPoll < POLL_MS) return;
  lastPoll = millis();

  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure(); // HTTPS simplu (fără verificare certificat) — pentru proiect

  HTTPClient https;
  String url = API_BASE + "/api/led/state";

  if (!https.begin(client, url)) {
    Serial.println("HTTPS begin failed");
    return;
  }

  int code = https.GET();
  if (code == 200) {
    String payload = https.getString();

    StaticJsonDocument<64> doc;
    auto err = deserializeJson(doc, payload);
    if (!err) {
      int led = doc["led"] | 0;
      digitalWrite(LED_PIN, led ? HIGH : LOW);
      Serial.printf("LED command = %d\n", led);
    } else {
      Serial.println("JSON parse error");
    }
  } else {
    Serial.printf("HTTP code: %d\n", code);
  }

  https.end();
}
