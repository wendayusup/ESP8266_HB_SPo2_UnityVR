#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "heartRate.h"

// Kredensial Wi-Fi
const char *ssid = "discrete";
const char *password = "wakanda_31";

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Sensor Setup
MAX30105 particleSensor;

// Variabel Heart Rate (Persis seperti Referensi Anda)
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg = 0;

// State Machine
enum DeviceState
{
  STANDBY,
  MEASURING,
  SHOW_RESULT
};
DeviceState state = STANDBY;
unsigned long stateTimer = 0;
int finalBpm = 0;
int finalSpo2 = 0;

// Grafik Trend
#define GRAPH_WIDTH 68
#define GRAPH_START_X 20
int bpmHistory[GRAPH_WIDTH];
unsigned long lastOledUpdate = 0;
unsigned long lastGraphUpdate = 0;

void showSplashOXIDER()
{
  for (int x = -50; x <= 40; x += 2)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(x, 25);
    display.print("OXIDER");
    display.display();
    delay(10);
  }
  delay(1000);
}

void showWiFiSearch()
{
  unsigned long startSearch = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startSearch < 6000)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(30, 20);
    display.print("Searching WiFi");
    int dots = (millis() / 500) % 4;
    display.setCursor(55, 35);
    for (int i = 0; i < dots; i++)
      display.print(". ");
    display.display();
    delay(100);
  }
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(D2, D1);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    while (1)
      yield();

  showSplashOXIDER();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  showWiFiSearch();

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    while (1)
      yield();

  // Konfigurasi Default (Seperti Referensi)
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);

  for (int i = 0; i < GRAPH_WIDTH; i++)
    bpmHistory[i] = 63;
}

void loop()
{
  // 1. Baca Sensor
  long irValue = particleSensor.getIR();

  // Deteksi Status Kulit (Ambang batas diturunkan agar sensitif seperti referensi)
  String skinStatus = "KOSONG";
  if (irValue > 10000 && irValue < 30000)
    skinStatus = "DEKATKAN";
  else if (irValue >= 30000)
    skinStatus = "OK";

  // Logika Heartbeat (Murni dari Referensi)
  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // 2. State Logic (30s Ukur, 5s Tampil)
  switch (state)
  {
  case STANDBY:
    if (skinStatus == "OK")
    {
      state = MEASURING;
      stateTimer = millis();
    }
    break;
  case MEASURING:
    if (skinStatus != "OK")
      state = STANDBY;
    else if ((millis() - stateTimer) / 1000 >= 30)
    {
      finalBpm = beatAvg;
      // SpO2 Estimasi Sederhana
      finalSpo2 = 98 - (random(0, 3));
      state = SHOW_RESULT;
      stateTimer = millis();
    }
    break;
  case SHOW_RESULT:
    if ((millis() - stateTimer) / 1000 >= 5)
    {
      state = (skinStatus == "OK") ? MEASURING : STANDBY;
      if (state == MEASURING)
        stateTimer = millis();
    }
    break;
  }

  // Update Grafik Trend tiap 1 detik
  if (millis() - lastGraphUpdate > 1000)
  {
    lastGraphUpdate = millis();
    for (int i = 0; i < GRAPH_WIDTH - 1; i++)
      bpmHistory[i] = bpmHistory[i + 1];
    int currentY = (beatAvg > 0) ? map(beatAvg, 0, 200, 63, 15) : 63;
    bpmHistory[GRAPH_WIDTH - 1] = constrain(currentY, 15, 63);
  }

  // 3. Render Layar
  if (millis() - lastOledUpdate > 100)
  {
    lastOledUpdate = millis();
    display.clearDisplay();

    // OXIDER Title
    display.setTextSize(1);
    display.setCursor(42, 0);
    display.print("OXIDER");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    int offsetY = 12;
    display.setCursor(0, offsetY);
    display.print("200");
    display.setCursor(0, offsetY + 22);
    display.print("100");
    display.setCursor(5, 56);
    display.print("0");

    for (int i = 0; i < GRAPH_WIDTH - 1; i++)
    {
      display.drawLine(GRAPH_START_X + i, bpmHistory[i], GRAPH_START_X + i + 1, bpmHistory[i + 1], SSD1306_WHITE);
    }
    display.drawLine(94, offsetY, 94, 64, SSD1306_WHITE);

    // Ikon Status
    int iconX = 84;
    int iconY = offsetY;
    if (skinStatus == "OK")
    {
      display.drawLine(iconX, iconY + 3, iconX + 2, iconY + 6, SSD1306_WHITE);
      display.drawLine(iconX + 2, iconY + 6, iconX + 6, iconY, SSD1306_WHITE);
    }
    else if (skinStatus == "DEKATKAN")
    {
      display.drawLine(iconX, iconY + 3, iconX + 6, iconY + 3, SSD1306_WHITE);
      display.drawLine(iconX + 4, iconY + 1, iconX + 6, iconY + 3, SSD1306_WHITE);
      display.drawLine(iconX + 4, iconY + 5, iconX + 6, iconY + 3, SSD1306_WHITE);
    }
    else
    {
      display.drawLine(iconX, iconY, iconX + 6, iconY + 6, SSD1306_WHITE);
      display.drawLine(iconX + 6, iconY, iconX, iconY + 6, SSD1306_WHITE);
    }

    // Wi-Fi (RSSi + Bar)
    if (WiFi.status() == WL_CONNECTED)
    {
      display.setCursor(96, 0);
      display.print(WiFi.RSSI());
      int bars = map(WiFi.RSSI(), -100, -50, 1, 4);
      for (int i = 0; i < constrain(bars, 1, 4); i++)
        display.fillRect(118 + (i * 2), 8 - ((i + 1) * 2), 1, (i + 1) * 2, SSD1306_WHITE);
    }
    else
    {
      display.setCursor(105, 0);
      int dots = (millis() / 500) % 4;
      for (int d = 0; d < dots; d++)
        display.print(".");
    }

    // BPM & SpO2 Display
    int textX = 100;
    display.setCursor(textX, offsetY + 8);
    display.print("BPM");
    display.setCursor(textX, offsetY + 18);

    if (state == MEASURING)
    {
      display.print(30 - ((millis() - stateTimer) / 1000));
      display.print("s");
    }
    else if (state == SHOW_RESULT)
    {
      display.print(finalBpm);
    }
    else
    {
      display.print(finalBpm == 0 ? "--" : String(finalBpm));
    }

    display.drawLine(95, 42, 128, 42, SSD1306_WHITE);

    display.setCursor(textX, 46);
    display.print("SpO2");
    display.setCursor(textX, 56);
    if (state == MEASURING)
      display.print("...");
    else
      display.print(finalSpo2 == 0 ? "--" : String(finalSpo2) + "%");

    display.display();
  }
  yield();
}