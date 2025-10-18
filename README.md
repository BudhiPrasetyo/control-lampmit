<h1 align="center">💡 IoT Lampu Control – Manual & Schedule System</h1>

<p align="center">
  <b>Smart Lamp Control with ESP32, Node-RED, and MIT App Inventor</b><br>
  ESP32 | Node-RED | MIT App Inventor | WebSocket | NTP Time
</p>

---

## 🧠 Overview

Proyek ini adalah sistem **IoT otomatis** untuk mengontrol lampu menggunakan **ESP32** dengan dua mode utama:
- **Manual Mode:** menyalakan dan mematikan lampu secara langsung dari aplikasi.
- **Schedule Mode:** menyalakan dan mematikan lampu otomatis berdasarkan jam tertentu.

Komunikasi antara MIT App Inventor → Node-RED → ESP32 dilakukan menggunakan **HTTP (POST)** dan **WebSocket**, tanpa memerlukan database tambahan.

---

## 🧩 Project Structure

| File | Deskripsi |
|------|------------|
| `esp32-controlmit.ino` | Kode utama ESP32 (manual + schedule) |
| `flow-nodered.json` | Flow Node-RED untuk komunikasi HTTP & WebSocket |
| `schedule_lampu.aia` | Aplikasi MIT App Inventor untuk kontrol lampu |
| `README.md` | Dokumentasi proyek |

---

## ⚙️ System Architecture

```text
[ MIT App Inventor ]
        │  (HTTP POST)
        ▼
[ Node-RED Server ]
        │  (WebSocket)
        ▼
[ ESP32 Device (Lampu) ]
```

**Alur kerja:**
1. MIT App Inventor mengirim perintah ke Node-RED (HTTP POST ke `/LAMPU`)
2. Node-RED meneruskan data ke ESP32 via WebSocket `/LAMPU`
3. ESP32 menjalankan perintah ON/OFF atau jadwal otomatis (schedule)
4. Lampu dikontrol secara real-time

---

## 📱 MIT App Inventor (Frontend)

**File:** `schedule_lampu.aia`

### ✨ Fitur:
- Tombol **ON / OFF** untuk kontrol manual  
- Input **jam ON** dan **jam OFF** untuk mode schedule  
- Tombol **Kirim Jadwal** untuk mengatur otomatisasi lampu  
- Mengirim data ke Node-RED dalam format:
  - Manual: `ON` / `OFF`
  - Schedule: `SCHEDULE:07:00,19:00`

> 💡 Data dikirim sebagai HTTP POST ke:  
> `http://<IP_NodeRED>:1880/LAMPU`

---

## 🧰 Backend – Node-RED

**File:** `flow-nodered.json`

### 📦 Fungsi Utama:
- **HTTP In Node (`/LAMPU`)** menerima perintah dari MIT App Inventor  
- **JSON Parse Node** membaca payload  
- **Function Node “Kirim ke ESP32”** mengubah data agar sesuai format WebSocket  
- **WebSocket Out Node (`/LAMPU`)** mengirimkan ke ESP32  
- **Debug Node** untuk monitoring pesan

### 🧾 Contoh Output Node-RED ke ESP32:
```
SCHEDULE:07:00,19:00
```
atau
```
ON
```
atau
```
OFF
```

---

## 🔌 ESP32 Device

**File:** `esp32-controlmit.ino`

### ⚙️ Fitur Utama:
- Tersambung ke WiFi dan sinkron waktu NTP (GMT+7)
- Terhubung ke Node-RED via WebSocket
- Menerima perintah:
  - `ON` → Lampu nyala
  - `OFF` → Lampu mati
  - `SCHEDULE:HH:MM,HH:MM` → Jadwal otomatis
- Menjalankan schedule hanya **sekali per waktu**, tidak spam
- Manual override: pengguna bisa ubah status manual tanpa mematikan schedule

### 🔧 Contoh Koneksi WebSocket
```cpp
const char* websocketServer = "192.168.1.100";
const int websocketPort = 1880;
const char* websocketPath = "/LAMPU";
```

### 📦 Library yang Digunakan
- `WiFi.h`
- `WebSocketsClient.h`
- `time.h`

---

## 🚀 Cara Menggunakan

### 1️⃣ Import Flow Node-RED
- Buka Node-RED → Menu → Import  
- Pilih file `flow-nodered.json`  
- Klik **Deploy**

### 2️⃣ Upload Kode ke ESP32
- Buka `esp32-controlmit.ino` di Arduino IDE / PlatformIO  
- Ganti SSID & password WiFi  
- Upload ke board ESP32  
- Buka Serial Monitor → pastikan koneksi sukses

### 3️⃣ Jalankan Aplikasi MIT App Inventor
- Install APK hasil compile `schedule_lampu.aia`
- Masukkan IP Node-RED
- Uji perintah **ON**, **OFF**, atau **Schedule**

---

## 🧩 Teknologi yang Digunakan

| Komponen | Teknologi |
|-----------|------------|
| IoT Device | ESP32 (Arduino Framework) |
| Backend | Node-RED (HTTP, WebSocket) |
| Frontend | MIT App Inventor |
| Waktu | NTP (pool.ntp.org) |

---

## 🧠 Format Pesan

| Mode | Format Pesan | Contoh |
|------|---------------|--------|
| Manual ON | `ON` | Menyalakan lampu |
| Manual OFF | `OFF` | Mematikan lampu |
| Schedule | `SCHEDULE:07:00,19:00` | Lampu ON jam 07:00, OFF jam 19:00 |

---

## 👨‍💻 Pengembang

**Author:** pklic  
🌐 GitHub: [https://github.com/BudhiPrasetyo](https://github.com/BudhiPrasetyo)  
📧 Email: *budhiprasetyoo28@email.com*  

---

## 🪪 Lisensi

Proyek ini dirilis di bawah lisensi **MIT** — bebas digunakan, dimodifikasi, dan dikembangkan dengan mencantumkan kredit pembuat aslinya.

---

<p align="center">⚡ Smart Lamp Control • 🌙 Schedule Automation • 🌍 Open Source</p>
