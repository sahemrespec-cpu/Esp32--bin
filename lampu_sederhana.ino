#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// --- KONFIGURASI WIFI (Lihat gambar QR Code kamu) ---
const char* ssid = "Starlink";
const char* password = "Gakngertiblas";

// --- KONFIGURASI TELEGRAM (Lihat gambar BotFather & ID BOT) ---
#define BOTtoken "8664935153:AAHnEBKdf5-qgAaIkRVnEnMQhbAWGg_rwlo"  
#define CHAT_ID "6326836122"

const int ledPin = 2; // Menggunakan LED internal ESP32 (GPIO 2)
bool ledStatus = LOW;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Interval pengecekan pesan (1 detik)
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    
    // Keamanan: Hanya balas jika Chat ID cocok dengan milikmu
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "User tidak dikenal!", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Halo " + from_name + ".\n";
      welcome += "Gunakan perintah berikut:\n\n";
      welcome += "/led_on : Menyalakan lampu\n";
      welcome += "/led_off : Mematikan lampu\n";
      welcome += "/status : Cek status lampu";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      digitalWrite(ledPin, HIGH);
      ledStatus = HIGH;
      bot.sendMessage(chat_id, "Lampu dinyalakan", "");
    }

    if (text == "/led_off") {
      digitalWrite(ledPin, LOW);
      ledStatus = LOW;
      bot.sendMessage(chat_id, "Lampu dimatikan", "");
    }

    if (text == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Lampu sedang NYALA", "");
      } else {
        bot.sendMessage(chat_id, "Lampu sedang MATI", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledStatus);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  client.setInsecure(); // Membuat koneksi tanpa perlu sertifikat SSL yang rumit

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
