#include <WiFi.h>
#include <WebSocketsClient.h>
#include <time.h>

const char* ssid = "NOC";
const char* password = "noc2023!";
const char* websocketServer = "192.168.223.21";  // IP Node-RED
const int websocketPort = 1880;
const char* websocketPath = "/LAMPU";

#define LAMPU_PIN 2  // GPIO2 untuk relay / LED

WebSocketsClient webSocket;

// Jadwal
String jamOn = "";
String jamOff = "";
bool useSchedule = false;

// Status lampu
bool lampuNyala = false;

// Mencegah jadwal retrigger di menit yang sama
String lastTriggered = "";

// Waktu NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

unsigned long lastCheck = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LAMPU_PIN, OUTPUT);
  digitalWrite(LAMPU_PIN, LOW);

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi Terhubung: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sinkronisasi waktu NTP...");
  delay(2000);
  printLocalTime();

  webSocket.begin(websocketServer, websocketPort, websocketPath);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  Serial.println("Menunggu pesan dari Node-RED...");
}

void loop() {
  webSocket.loop();

  if (useSchedule && millis() - lastCheck >= 1000) {
    lastCheck = millis();
    cekJadwal();
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Terputus");
      break;
    case WStype_CONNECTED:
      Serial.println("[WebSocket] Terhubung ke Node-RED");
      break;
    case WStype_TEXT:
      handleMessage((char*)payload);
      break;
  }
}

void handleMessage(String msg) {
  msg.trim();
  msg.toUpperCase();
  Serial.print("Pesan diterima: ");
  Serial.println(msg);

  // Format: SCHEDULE:HH:MM,HH:MM
  if (msg.startsWith("SCHEDULE:")) {
    String data = msg.substring(9);
    int commaIndex = data.indexOf(',');
    if (commaIndex != -1) {
      jamOn = data.substring(0, commaIndex);
      jamOff = data.substring(commaIndex + 1);
      jamOn.trim();
      jamOff.trim();
      useSchedule = true;
      lastTriggered = ""; // reset jadwal
      Serial.println("Mode: Schedule aktif");
      Serial.print("Jam ON : "); Serial.println(jamOn);
      Serial.print("Jam OFF: "); Serial.println(jamOff);
      return;
    }
  }

  // Manual ON/OFF — tetap bisa di tengah schedule
  if (msg == "ON") {
    nyalakanLampu(true, "Manual");
  } 
  else if (msg == "OFF") {
    nyalakanLampu(false, "Manual");
  } 
  else {
    Serial.println("Pesan tidak dikenali");
  }
}

void cekJadwal() {
  String waktu = getCurrentTime();

  // Jalankan hanya jika waktu berganti menit (hindari spam)
  if (waktu == lastTriggered) return;

  if (waktu == jamOn) {
    nyalakanLampu(true, "Schedule");
    lastTriggered = waktu;
  }
  else if (waktu == jamOff) {
    nyalakanLampu(false, "Schedule");
    lastTriggered = waktu;
  }
}

void nyalakanLampu(bool nyala, String sumber) {
  digitalWrite(LAMPU_PIN, nyala ? HIGH : LOW);
  lampuNyala = nyala;
  Serial.print("Lampu ");
  Serial.print(sumber);
  Serial.print(": ");
  Serial.println(nyala ? "ON" : "OFF");
}

String getCurrentTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "";
  }
  char buffer[6];
  sprintf(buffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  return String(buffer);
}

void printLocalTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.printf("Waktu sekarang: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  }
}
