#ifndef GPRS_CONFIG_H
#define GPRS_CONFIG_H
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include "oledConfig.h"
#define MODEM_RX 3 // RX ke TX SIM800L
#define MODEM_TX 1 // TX ke RX SIM800L
#define MODEM_BAUD 9600
extern MyOLED myOled;
// Konfigurasi APN Telkomsel
const char apn[] = "internet"; // APN Telkomsel
const char gprsUser[] = "";    // Username Telkomsel (kosong)
const char gprsPass[] = "";    // Password Telkomsel (kosong)

SoftwareSerial modemSerial(MODEM_RX, MODEM_TX);
TinyGsm modem(modemSerial);
TinyGsmClient gprsClient(modem);
void connectGPRS()
{
    Serial.println("Menginisialisasi modem...");
    myOled.displayText("Menginisialisasi modem", 1);
    if (!modem.restart())
    {
        Serial.println("Gagal menginisialisasi modem. Periksa koneksi.");
        myOled.displayText("gagal menginisialisasi", 1);
        while (true)
            ;
    }
    Serial.println("Modem siap!");
    myOled.displayText("modem siap", 1, 15);
    delay(1000);
    String operatorName = modem.getOperator();
    Serial.print("Operator: ");
    Serial.println(operatorName);
    delay(1000);

    Serial.print("Menghubungkan ke jaringan GPRS dengan APN: ");
    Serial.println(apn);
    myOled.displayText("Menghubungkan GPRS", 1);

    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        Serial.println("Gagal terhubung ke jaringan GPRS.");
        myOled.displayText("Gagal terhubung", 1);
        while (true)
            ;
    }
    Serial.println("Berhasil terhubung ke jaringan GPRS!");
    modem.localIP();
    Serial.print("IP Address: ");
    Serial.println(modem.localIP());
    myOled.displayText("GPRS terhubung", 1);
}

#endif