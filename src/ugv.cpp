#include <Arduino.h> //basic arduino functions (serial, delay, setup/loop)
#include <WiFi.h> // controls ESP32 wifi raduo mode to STA or AP mode. espnow uses this radio
#include <esp_now.h> // espno2 api. it has init, send/recieve callbacks, peer management

// sending telemetry packets from uav to ugv
// STRUCT - DECODE
// packed cause it prevents compiler from inserting padding bytes (alignment gaps)
// this keeps the binary layout identical across devices/compilers

typedef struct __attribute__((packed))
{
 uint32_t sequence;  //sequence number increments every packet
 uint32_t timestamp_ms; // sender timestamp in milliseconds
 float velocity_x; //simulated velocity x
 float velocity_y; //simulated velocity y 
 bool markerDetected; //marker detected by uav // flag
 bool emergencyStop; //emergency stop flag
 uint8_t cmd; //command field for ugv
} telemetryPacket;

// this callback runs automatically whenver an ESPNOW packet is recieved
static void whenDataIsRecieved(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    // 1) validating expected size
    if (len != (int)sizeof(telemetryPacket))
    {
        Serial.printf("RX Wrong size: %d (expected %d)\n", len, (int)sizeof(telemetryPacket));
        return;
    }

    // 2) copying bytes into a struct instance (safe cause we verified length)
    telemetryPacket packet;
    memcpy(&packet, incomingData, sizeof(telemetryPacket));

    // 3) print who sent it (MAC) + decoded fields
    Serial.printf("Recieved from MAC: %02X:%02X:%02X:%02X:%02X:%02X | ", 
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    Serial.printf("Seq: %u | TS: %u | Vel: (%.2f, %.2f) | Marker: %d | EmStop: %d | Cmd: %d\n",
        (unsigned long)packet.sequence, 
        (unsigned long)packet.timestamp_ms,
        packet.velocity_x,
        packet.velocity_y,
        packet.markerDetected ? 1 : 0,
        packet.emergencyStop ? 1 : 0,
        packet.cmd);
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    WiFi.mode(WIFI_STA); //setting wifi in station mode //putting wifi radio in station mode which is required for espnow. we are not connecting to any router here. this enables STA interface

    if (esp_now_init != ESP_OK)
    {
        Serial.println("ESP-NOW init Failed");
        while(true) delay(1000);
    }

    esp_now_register_recv_cb(whenDataIsRecieved);

    Serial.println("Reciever (UGV) ready! My MAC is: ");
    Serial.println(WiFi.macAddress());

    Serial.println("Waiting for telemetry from UAV...");
    Serial.printf("Expected Package Size: %d bytes\n", (int)sizeof(telemetryPacket));

}

void loop()
{
    // nothing to do here - all logic is in the callback
    delay(1000);
}

