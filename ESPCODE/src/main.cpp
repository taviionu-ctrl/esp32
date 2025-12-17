#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Sarbatori fericite";
const char* password = "parola123";

// URL-ul serverului Render
String API_BASE = "https://esp32-4do0.onrender.com";

const int LED_PIN = 4; // GPIO4 (LED-ul tău)

unsigned long lastPoll = 0;
const unsigned long POLL_MS = 1500;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Conectare Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {  // Timeout după 10 încercări
    delay(400);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("ESP32 local IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed! Restarting...");
    ESP.restart();  // Dacă nu se poate conecta, repornește ESP32
  }
}

void loop() {
  if (millis() - lastPoll < POLL_MS) return;  // Încearcă să faci cererea o dată la 1.5 secunde
  lastPoll = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, retrying...");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); // HTTPS simplu (fără verificare certificat) — pentru proiect

  HTTPClient https;
  
  // Citirea stării LED-ului
  String urlState = API_BASE + "/api/led/state";
  if (!https.begin(client, urlState)) {
    Serial.println("HTTPS begin failed for state request");
    return;
  }

  int code = https.GET();
  if (code == 200) {
    String payload = https.getString();

    StaticJsonDocument<64> doc;
    auto err = deserializeJson(doc, payload);
    if (!err) {
      int led = doc["led"] | 0; // Dacă nu există "led", folosește valoarea 0 (LED OFF)
      digitalWrite(LED_PIN, led ? HIGH : LOW);
      Serial.printf("LED command = %d\n", led);
    } else {
      Serial.println("JSON parse error");
    }
  } else {
    Serial.printf("HTTP error code: %d\n", code);
  }

  // Controlul LED-ului: Aprinde LED-ul
  String urlOn = API_BASE + "/api/led/on";
  if (!https.begin(client, urlOn)) {
    Serial.println("HTTPS begin failed for on request");
    return;
  }

  code = https.GET();
  if (code == 200) {
    Serial.println("LED turned ON");
  } else {
    Serial.printf("HTTP error code for ON: %d\n", code);
  }

  // Controlul LED-ului: Stinge LED-ul
  String urlOff = API_BASE + "/api/led/off";
  if (!https.begin(client, urlOff)) {
    Serial.println("HTTPS begin failed for off request");
    return;
  }

  code = https.GET();
  if (code == 200) {
    Serial.println("LED turned OFF");
  } else {
    Serial.printf("HTTP error code for OFF: %d\n", code);
  }

  https.end();

  delay(500);  // Delay suplimentar pentru a reduce frecvența cererilor
}
