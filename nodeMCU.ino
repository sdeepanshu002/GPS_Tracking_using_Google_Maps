#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Replace with your WiFi credentials
const char *ssid = "your_wifi_ssid";
const char *password = "your_wifi_password";

// Replace with your Discord webhook URL
const char *webhookUrl = "https://discord.com/api/webhooks/your_webhook_id/your_webhook_token";

// GPS module connection (TX, RX)
SoftwareSerial gpsSerial(4, 5); // Connect TX of GPS module to D4 and RX to D5

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        sendToDiscord(gps.location.lat(), gps.location.lng());
      }
    }
  }
}

void sendToDiscord(float latitude, float longitude) {
  String message = "GPS Coordinates: Lat " + String(latitude, 6) + ", Lon " + String(longitude, 6);

  // Construct the Google Maps link
  String mapsLink = "https://www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6);

  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");

  // Construct the JSON payload
  String jsonPayload = "{\"content\":\"" + message + "\\n" + mapsLink + "\"}";

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.println("Message sent to Discord successfully");
    // Open Google Maps link in a web browser
    Serial.println("Google Maps Link: " + mapsLink);
  } else {
    Serial.println("Error sending message to Discord");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(5000); // Delay for 5 seconds before sending the next update
}