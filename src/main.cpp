#include <Arduino.h>
#include "RfidConfig.h"
#include "OledConfig.h"
#include "BuzzerConfig.h"
#include "MqttConfig.h"
#include "gprsConfig.h"
#include "wifiConfig.h"
#include <ezButton.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
ezButton button(0);
MyOLED myOled;
Buzzer buzzer(2);
const int potPin = A0;
int potValue = 0;
int confirmedValue = 0;
bool confirmed = false;
const int selisih = 3;
int volume;
String jsonString;
String mqttPayload;
static bool lastNetworkStatus = false;

void setup()
{
  ArduinoOTA.begin();
  ArduinoOTA.setHostname("Mobile-Esp");
  Serial.begin(115200);
  buzzer.beep(1, 110);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  button.setCountMode(COUNT_FALLING);
  buzzer.beep(1, 110);
  if (!myOled.begin())
  {
    Serial.println(F("Gagal menginisialisasi OLED!"));
    while (true)
      ; // Hentikan program jika gagal
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    myOled.displayText("Hubungkan Wifi!", 2);
    connectToWiFi();
    if (WiFi.status() == WL_CONNECTED)
    {
      break;
    }
  }
  // connectGPRS();
  setupMQTT();
  initRfid();
}
bool isButtonPressed()
{
  button.loop();             // Perbarui status tombol
  return button.isPressed(); // Kembalikan true jika tombol ditekan
}
void loop()
{
  ArduinoOTA.handle();
  client.loop();
  button.loop();

  bool currentNetworkStatus = modem.isNetworkConnected();

  // if (currentNetworkStatus != lastNetworkStatus)
  // {
  //   if (currentNetworkStatus)
  //   {
  //     Serial.println("Jaringan terhubung.");
  //     myOled.displayText("Jaringan Terhubung", 1);
  //   }
  //   else
  //   {
  //     Serial.println("Jaringan terputus.");
  //     myOled.displayText("Jaringan Terputus", 1);
  //   }
  //   lastNetworkStatus = currentNetworkStatus;
  // }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Tidak Terkoneksi WiFi!");
    reconnectWiFi();
  }
  if (!client.connected())
  {
    setupMQTT();
  }
  String tagData = readTag();
  if (tagData != "")
  {
    buzzer.beep(3, 110);
    if (publishData(tagData))
    {
      Serial.println("UID berhasil dikirim. Menunggu balasan...");
      unsigned long startTime = millis();
      bool responseReceived = false;

      while (millis() - startTime < 2000)
      {
        client.loop(); // Proses pesan MQTT yang masuk

        if (!mqttPayload.isEmpty())
        {
          if (mqttPayload == "400")
          {
            Serial.println("Data sudah terdaftar hari ini.");
            myOled.displayText("Data Sudah Terdaftar Hari ini", 1);
            mqttPayload = "";
            responseReceived = true; // Balasan diterima
            break;                   // Keluar dari loop
          }
          else if (mqttPayload == "500")
          {
            Serial.println("Data tidak terdaftar.");
            myOled.displayText("Data Tidak Terdaftar", 1);
            mqttPayload = "";
            responseReceived = true; // Balasan diterima
            break;                   // Keluar dari loop
          }
          else if (mqttPayload == "200")
          {
            Serial.println("Data diteruskan.");
            myOled.displayText("Data diteruskan", 2);
            unsigned long startTime2 = millis();
            while (millis() - startTime2 < 30000)
            {                                          // Tunggu hingga 20 detik
              int rawVal = analogRead(potPin);         // Baca potensiometer
              potValue = map(rawVal, 0, 1023, 0, 100); // Peta nilai 0-100
              Serial.print("Potensiometer: ");
              Serial.println(potValue);
              myOled.displayText("Volume: ", 2);
              myOled.displayText(String(potValue).c_str(), 2, 21);
              if ((volume - potValue >= selisih) || (potValue - volume >= selisih))
              {
                volume = potValue;
                buzzer.beep(1, 30);
              }

              if (isButtonPressed())
              { // Jika tombol ditekan
                buzzer.beep(1, 70);
                confirmedValue = potValue; // Simpan nilai yang dikonfirmasi
                confirmed = true;          // Tandai sebagai dikonfirmasi
                break;                     // Keluar dari loop
              }
              delay(50);
            }
            if (confirmed)
            { // Jika nilai dikonfirmasi
              Serial.println("Nilai Dikonfirmasi: " + String(confirmedValue));
              myOled.displayText("Nilai Dikonfirmasi: ", 2);
              myOled.displayText(String(confirmedValue).c_str(), 2);
              DynamicJsonDocument doc(256); // Ukuran buffer JSON
              doc["uid"] = tagData;
              doc["volume"] = confirmedValue;
              serializeJson(doc, jsonString);
              if (publishData(jsonString))
              {
                Serial.println("Nilai dikirim ke broker MQTT.");
                myOled.displayText("Nilai dikirim ke broker MQTT", 2);
                confirmed = false; // Reset flag dikonfirmasi
              }
              else
              {
                Serial.println("Gagal mengirim nilai ke broker MQTT.");
                myOled.displayText("Gagal mengirim nilai ke broker MQTT", 2);
              }
            }
            else // waktu sudah habis
            {
              Serial.println("Waktu habis! Tidak ada input tombol.");
              myOled.displayText("Waktu habis!", 2);
              buzzer.beep(2, 110);
            }
            mqttPayload = "";
            responseReceived = true;
            break; // Keluar dari loop
          }
          else
          {
            Serial.println("Balasan tidak dikenali: " + mqttPayload);
            mqttPayload = "";
            responseReceived = true; // Tetap tandai sebagai diterima untuk keluar dari loop
            break;                   // Keluar dari loop
          }
        }
      }

      if (!responseReceived)
      {
        Serial.println("Tidak ada balasan dalam batas waktu.");
        myOled.displayText("Timeout! Tidak Ada Balasan", 2);
      }
    }
    else
    {
      Serial.println("Gagal mengirim UID ke broker MQTT.");
    }
  }
}