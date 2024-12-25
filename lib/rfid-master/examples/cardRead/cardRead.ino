/**
* Read a card using a mfrc522 reader on your SPI interface
* Pin layout should be as follows (on Arduino Uno):
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 10
* RST: Pin 9
*
* Script is based on the script of Miguel Balboa. 
* New cardnumber is printed when card has changed. Only a dot is printed
* if card is the same.
*
* @version 0.1
* @author Henri de Jong
* @since 06-01-2013
*/

#include <SPI.h>
#include <RFID.h>

#define SS_PIN 5
#define RST_PIN 22

RFID rfid(SS_PIN, RST_PIN);  // Objek RFID

// Setup variables:
String tagData = "";

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.init();
  Serial.println("RFID Reader diinisialisasi!");
}

void loop() {

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      // Mengonversi data ID tag ke format String
      tagData += String(rfid.serNum[0], HEX);
      tagData += String(rfid.serNum[1], HEX);
      tagData += String(rfid.serNum[2], HEX);
      tagData += String(rfid.serNum[3], HEX);

      // Menampilkan hasil di Serial Monitor
      Serial.println(" ");
      Serial.println("Card found");
      Serial.println("Cardnumber:");
      Serial.print("Dec: ");
      Serial.print(rfid.serNum[0], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[1], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[2], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[3], HEX);
      Serial.println();
    }
  }

  rfid.halt();
}
