#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#include <MQTT.h>
#include "GprsConfig.h"
#include "oledConfig.h"
#include "wifiConfig.h"
extern String mqttPayload;
const char *mqtt_broker = "msib7ldre.cloud.shiftr.io";
const int mqtt_port = 1883;
const char *mqtt_user = "msib7ldre";  // Username Shifter.io
const char *mqtt_pass = "bsimin1234"; // Password Shifter.io
extern MyOLED myOled;
WiFiClient net;
MQTTClient client;
String pathReceive = "bsi/data/datapelanggan/espabsensi";
String pathSend = "bsi/data/datapelanggan/espmobile";
void connectToMQTT()
{
    Serial.print("Menghubungkan ke broker MQTT...");
    myOled.displayText("Menghubungkan Broker Mqtt", 1);
    while (!client.connect("client_id_espbsimobile", mqtt_user, mqtt_pass))
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nTerhubung ke broker MQTT!");
    myOled.displayText("Terhubung", 1);
    myOled.displayText("Subcribe", 1, 15);
    myOled.displayText(pathReceive.c_str(), 1, 30);
    client.subscribe(pathReceive);
    Serial.println("Subscribed ke topik: " + pathReceive);
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("Pesan diterima: " + topic + " - " + payload);
    mqttPayload = payload;
    if (topic == pathReceive)
    {
        Serial.println("Data dari " + pathReceive + ": " + payload);
    }
    else
    {
        Serial.println("Topik tidak dikenali: " + topic);
    }
}

void setupMQTT()
{
    client.begin(mqtt_broker, mqtt_port, net); // Gunakan objek WiFiClient untuk MQTT
    client.onMessage(messageReceived);         // Set callback untuk pesan masuk
    connectToMQTT();
}

bool publishData(const String &data, const String &topic = pathSend, int qos = 2)
{
    if (client.connected())
    {
        bool success = client.publish(topic, data, false, qos); // QoS ditentukan di sini
        if (success)
        {
            Serial.println("Data berhasil dikirim ke topik " + topic + ": " + data);
            myOled.displayText("Data berhasil dikirim", 1);
            return true;
        }
        else
        {
            Serial.println("Gagal mengirim data ke topik: " + topic);
            return false;
        }
    }
    else
    {
        Serial.println("Koneksi ke MQTT terputus, mencoba menyambungkan kembali...");
        connectToMQTT();
        return false;
    }
}

#endif