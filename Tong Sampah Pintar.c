#include <ESP32Servo.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT22
#define TRIG_PIN 2
#define ECHO_PIN 4
#define SERVO_PIN 22
#define DISTANCE_THRESHOLD 15  // Jarak dalam cm untuk mendeteksi tangan

DHT dht(DHTPIN, DHTTYPE);
Servo servo;

float duration_us, distance_cm;
unsigned long lastOpenedTime = 0;
const unsigned long openDuration = 5000;  // Waktu dalam ms sebelum menutup kembali

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
  dht.begin();

  // Memastikan servo berada di posisi menutup pada awalnya
  servo.write(0);
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Memeriksa apakah pembacaan sensor DHT valid
  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca dari sensor DHT!");
    return;
  }

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;

  // Debugging jarak dan posisi servo
  Serial.print("Jarak terukur: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  if (distance_cm < DISTANCE_THRESHOLD) {
    Serial.println("Jarak di bawah ambang batas, membuka penutup");
    servo.write(90);
    lastOpenedTime = millis();
  } 

  // Menutup penutup secara otomatis setelah waktu tertentu
  if (millis() - lastOpenedTime > openDuration) {
    Serial.println("Menutup penutup secara otomatis");
    servo.write(0);
  }

  Serial.print("Suhu = ");
  Serial.print(t);
  Serial.println(" C");
  Serial.print("Kelembaban = ");
  Serial.print(h);
  Serial.println(" %");

  delay(500);
}
