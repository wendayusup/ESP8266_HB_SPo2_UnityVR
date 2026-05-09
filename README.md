# OXIDER OS (HBSPO UnityVR)

[English](#english) | [Bahasa Indonesia](#bahasa-indonesia)

---

<a name="english"></a>
## English Version

OXIDER OS is a high-performance health monitoring firmware designed for ESP8266 (Wemos D1 Mini) and the MAX30102 sensor. It features a professional user interface inspired by modern wearable devices and robust signal processing algorithms to provide stable heart rate and oxygen saturation (SpO2) readings.

### Key Features
- Premium UX/UI: High-refresh-rate OLED interface with a centralized branding title and organized data layout.
- Dynamic Booting Sequence: Animated splash screens for branding and real-time WiFi connection status.
- Smart Measurement Protocol:
  - 30-Second Sampling: A specialized data collection window for high-confidence results.
  - Auto-Looping: Continuous measurement cycles with 5-second result persistence.
- BPM Trend Graph: Real-time visualization of the last 60+ seconds of heart rate data.
- Background Networking: Non-blocking WiFi reconnection and signal strength (RSSI) monitoring.

### Hardware Requirements
- Core: Wemos D1 Mini (ESP8266)
- Sensor: MAX30102 High-Sensitivity Pulse Oximeter
- Display: 0.96" SSD1306 OLED (128x64)

### Installation
1. Open the project in VS Code with the PlatformIO extension.
2. Configure your WiFi credentials in src/main.cpp.
3. Build and Upload to your device.

---

<a name="bahasa-indonesia"></a>
## Bahasa Indonesia

OXIDER OS adalah firmware monitoring kesehatan berperforma tinggi yang dirancang untuk ESP8266 (Wemos D1 Mini) dan sensor MAX30102. Sistem ini memiliki antarmuka profesional yang terinspirasi dari perangkat wearable modern serta algoritma pemrosesan sinyal yang kokoh untuk memberikan pembacaan detak jantung dan saturasi oksigen (SpO2) yang stabil.

### Fitur Unggulan
- UX/UI Premium: Antarmuka OLED dengan refresh rate tinggi, judul branding terpusat, dan tata letak data yang terorganisir.
- Sekuens Booting Dinamis: Splash screen beranimasi untuk identitas brand dan status koneksi WiFi real-time.
- Protokol Pengukuran Cerdas:
  - Sampling 30 Detik: Jendela pengumpulan data khusus untuk hasil dengan tingkat kepercayaan tinggi.
  - Auto-Looping: Siklus pengukuran berkelanjutan dengan retensi hasil selama 5 detik.
- Grafik Tren BPM: Visualisasi real-time dari riwayat detak jantung selama 60+ detik terakhir.
- Jaringan Latar Belakang: Re-koneksi WiFi non-blocking dan pemantauan kekuatan sinyal (RSSI).

### Kebutuhan Perangkat Keras
- Inti: Wemos D1 Mini (ESP8266)
- Sensor: MAX30102 High-Sensitivity Pulse Oximeter
- Layar: 0.96" SSD1306 OLED (128x64)

### Cara Instalasi
1. Buka proyek menggunakan VS Code dengan ekstensi PlatformIO.
2. Masukkan kredensial WiFi Anda pada file src/main.cpp.
3. Build dan Upload ke perangkat Anda.

---

