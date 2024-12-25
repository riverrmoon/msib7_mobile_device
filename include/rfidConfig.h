#ifndef RFID_CONFIG_H
#define RFID_CONFIG_H

#include <SPI.h>
#include <RFID.h>

#define SS_PIN 15
#define RST_PIN 0

RFID rfid(SS_PIN, RST_PIN); // Objek RFID

void initRfid()
{
    SPI.begin();
    rfid.init();
    Serial.println("RFID Reader READY");
}

String readTag()
{
    String tagData = "";
    if (rfid.isCard())
    {
        if (rfid.readCardSerial())
        {
            for (byte i = 0; i < 4; i++)
            {
                tagData += String(rfid.serNum[i], HEX);
            }
            Serial.println(" ");
            Serial.println("Card found");
            Serial.println("Cardnumber:");
            Serial.println(tagData);
        }
    }
    rfid.halt();

    return tagData;
}

#endif