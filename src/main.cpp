// TFT Gauges for ESP32 - Pitch, Compass, Altitude, Debug Info
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Shared SPI pins
#define TFT_MOSI 17
#define TFT_SCLK 23
#define TFT_DC   19
#define TFT_RST  18

// CS pins for displays
#define TFT_CS2 33  // Display 2 - Pitch
#define TFT_CS3 32  // Display 3 - Compass Rose
#define TFT_CS4 25  // Display 4 - Altimeter
#define TFT_CS5 21  // Display 5 - Debug Info (IP, MAC, X-Plane Status)

WiFiUDP udp;
const int localPort = 49000;

TFT_eSPI tft = TFT_eSPI();

float heading = 0.0, lastHeading = -1;
float pitch = 0.0, lastPitch = -999;
float altitude = 0.0, lastAltitude = -999;
bool dataReceived = false, lastDataReceived = false;
String lastIP = "", lastMAC = "";

void selectScreen(uint8_t cs) {
  Serial.print("Switching to CS pin: "); Serial.println(cs);
  digitalWrite(TFT_CS2, HIGH);
  digitalWrite(TFT_CS3, HIGH);
  digitalWrite(TFT_CS4, HIGH);
  digitalWrite(TFT_CS5, HIGH);
  digitalWrite(cs, LOW);
  tft.setRotation(0);
}

void drawPitchIndicator(float pitchVal) {
  Serial.println("Drawing Pitch");
  tft.fillScreen(TFT_BLACK);
  int centerY = 120;
  int lineY = centerY - pitchVal * 2;
  tft.drawLine(60, lineY, 180, lineY, TFT_BLUE);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(80, 10);
  tft.print("Pitch:");
  tft.println((int)pitchVal);
}

void drawCompassRose(float hdg) {
  Serial.println("Drawing Compass");
  tft.fillScreen(TFT_BLACK);
  int centerX = 120, centerY = 120, radius = 100;
  tft.drawCircle(centerX, centerY, radius, TFT_WHITE);
  float angle = radians(hdg - 90);
  int x = centerX + cos(angle) * (radius - 10);
  int y = centerY + sin(angle) * (radius - 10);
  tft.drawLine(centerX, centerY, x, y, TFT_RED);
  tft.setTextSize(2);
  tft.setCursor(70, 10);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print("HDG:");
  tft.print((int)hdg);
}

void drawAltitudeGauge(float alt) {
  Serial.println("Drawing Altitude");
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(30, 100);
  tft.println("Altitude");
  tft.setTextSize(4);
  tft.setCursor(50, 150);
  tft.println((int)alt);
}

void drawDebugInfo(String ip, String mac, bool connected) {
  Serial.println("Drawing Debug Info");
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(10, 40);
  tft.println("IP: " + ip);
  tft.println("MAC: " + mac);
  tft.setTextSize(3);
  tft.setTextColor(connected ? TFT_GREEN : TFT_RED, TFT_BLACK);
  tft.setCursor(10, 140);
  tft.println(connected ? "X-Plane: YES" : "X-Plane: NO");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  pinMode(TFT_CS2, OUTPUT);
  pinMode(TFT_CS3, OUTPUT);
  pinMode(TFT_CS4, OUTPUT);
  pinMode(TFT_CS5, OUTPUT);

  digitalWrite(TFT_CS2, HIGH);
  digitalWrite(TFT_CS3, HIGH);
  digitalWrite(TFT_CS4, HIGH);
  digitalWrite(TFT_CS5, HIGH);

  selectScreen(TFT_CS2); tft.init(); digitalWrite(TFT_CS2, HIGH);
  WiFi.begin("WrumWrum2", "A407188ecd3");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(localPort);
  Serial.println("UDP listening on port " + String(localPort));
}

void readUDP() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    dataReceived = true;
    char packetBuffer[1472];
    udp.read(packetBuffer, 1472);

    if (strncmp(packetBuffer, "DATA", 4) == 0) {
      for (int i = 5; i < packetSize; i += 36) {
        int index = *(int*)&packetBuffer[i];
        if (index == 17) {
          heading = *(float*)&packetBuffer[i + 4];
          pitch = *(float*)&packetBuffer[i + 8];
        }
        else if (index == 20) altitude = *(float*)&packetBuffer[i + 4];
      }
    }
  }
}

void loop() {
  readUDP();
  String currentIP = WiFi.localIP().toString();
  String currentMAC = WiFi.macAddress();

  if (pitch != lastPitch) {
    selectScreen(TFT_CS2);
    drawPitchIndicator(pitch);
    lastPitch = pitch;
  }

  if (heading != lastHeading) {
    selectScreen(TFT_CS3);
    drawCompassRose(heading);
    lastHeading = heading;
  }

  if (altitude != lastAltitude) {
    selectScreen(TFT_CS4);
    drawAltitudeGauge(altitude);
    lastAltitude = altitude;
  }

  if (currentIP != lastIP || currentMAC != lastMAC || dataReceived != lastDataReceived) {
    selectScreen(TFT_CS5);
    drawDebugInfo(currentIP, currentMAC, dataReceived);
    lastIP = currentIP;
    lastMAC = currentMAC;
    lastDataReceived = dataReceived;
  }

  dataReceived = false;
  delay(20);
}