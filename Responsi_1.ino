
#include <WiFi.h>

// =====================================
// KONFIGURASI WIFI
// =====================================
const char* ssid = "MR-FK-01";
const char* password = "adzanmasjid";

// =====================================
// KONFIGURASI PIN SENSOR
// =====================================
#define TRIG_PIN 5
#define ECHO_PIN 18
#define PIR_PIN 19

// Batas jarak objek (cm)
const float BATAS_JARAK = 100.0;

// Interval pembacaan sensor
unsigned long waktuSebelumnya = 0;
const unsigned long interval = 1000;

// =====================================
// FUNGSI MEMBACA JARAK ULTRASONIC
// =====================================
float bacaJarak() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long durasi = pulseIn(ECHO_PIN, HIGH, 30000);

  if (durasi == 0) {
    return -1;
  }

  float jarak = durasi * 0.0343 / 2;

  return jarak;
}

// =====================================
// FUNGSI KONEKSI WIFI
// =====================================
void koneksiWiFi() {
  Serial.println();
  Serial.println("Menghubungkan ke WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int percobaan = 0;

  while (WiFi.status() != WL_CONNECTED &&
         percobaan < 40) {
    delay(500);
    Serial.print(".");
    percobaan++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Gagal terhubung ke WiFi.");
    Serial.println("Sistem tetap menjalankan sensor.");
  }
}

// =====================================
// SETUP
// =====================================
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  Serial.println("================================");
  Serial.println(" SISTEM MONITORING KEAMANAN IoT ");
  Serial.println("================================");

  koneksiWiFi();

  Serial.println();
  Serial.println("Menunggu sensor PIR siap...");
  delay(30000);

  Serial.println("Sensor siap digunakan!");
}

// =====================================
// LOOP
// =====================================
void loop() {

  // Cek koneksi WiFi
  static unsigned long waktuWiFi = 0;

  if (WiFi.status() != WL_CONNECTED &&
      millis() - waktuWiFi >= 10000) {
    waktuWiFi = millis();
    Serial.println("Mencoba menghubungkan WiFi kembali...");
    WiFi.reconnect();
  }

  // Pembacaan sensor setiap 1 detik
  if (millis() - waktuSebelumnya >= interval) {
    waktuSebelumnya = millis();

    // Membaca sensor
    float jarak = bacaJarak();
    int gerakan = digitalRead(PIR_PIN);

    // Menampilkan hasil sensor
    Serial.println();
    Serial.println("===== MONITORING IoT =====");

    if (jarak >= 0) {
      Serial.print("Jarak Objek : ");
      Serial.print(jarak);
      Serial.println(" cm");
    } else {
      Serial.println("Jarak Objek : Tidak terdeteksi");
    }

    if (gerakan == HIGH) {
      Serial.println("Sensor PIR  : GERAKAN TERDETEKSI");
    } else {
      Serial.println("Sensor PIR  : TIDAK ADA GERAKAN");
    }

    // =================================
    // LOGIKA PENGGABUNGAN SENSOR
    // =================================

    if (gerakan == HIGH && jarak > 0 &&
        jarak <= BATAS_JARAK) {

      Serial.println("STATUS      : OBJEK BERGERAK TERDETEKSI");

    } else if (gerakan == HIGH) {

      Serial.println("STATUS      : ADA GERAKAN");

    } else if (jarak > 0 && jarak <= BATAS_JARAK) {

      Serial.println("STATUS      : ADA OBJEK DALAM JANGKAUAN");

    } else {

      Serial.println("STATUS      : KONDISI NORMAL");
    }

    // Status WiFi
    Serial.print("Koneksi WiFi: ");

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("TERHUBUNG");
    } else {
      Serial.println("TERPUTUS");
    }

    Serial.println("==========================");
  }
}

