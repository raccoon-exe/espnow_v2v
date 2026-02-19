#include <Arduino.h> // serial, delay, setup/loop
#include <WiFi.h> // wifi radio mode control like STA or AP mode
#include <esp_now.h> // espnow api. init, send/recieve callbacks, peer management

// mac address of esp32_1 = F8:B3:B7:20:25:A8
// mac address of esp32_2 = F8:B3:B7:20:69:C0

// giving the uav the mac adress of UGV as raw bytes
// so the ESP-NOW targets devices by MAC address (no Ip, no router, no wifi network)
// so we need to manually add the peer (UGV) to the ESP-NOW network
// this is done using esp_now_add_peer()
uint8_t UGV_MAC[] = {0xF8, 0xB3, 0xB7, 0x20, 0x69, 0xC0}; //mac address of UGV as raw bytes

// this callback runs after a send attempt finishes
// it tells us whether the radio reported success or failure
static void afterDataSent (const uint8_t *mac, esp_now_send_status_t status)
{
  // mac = the mac address of the device we tried to send to
  // status = ESP_NOW_SEND_SUCCESS or ESP_NOW_SEND_FAIL
  // printing the mac address of the device we tried to send to
  Serial.printf("Sending to MAC: %02X:%02X:%02X:%02X:%02X:%02X | ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (status == ESP_NOW_SEND_SUCCESS) 
  Serial.println("Success");
  else Serial.println("Fail");
}

void setup()
{
  Serial.begin(115200);  //start the serial monitor
  delay(500); //give time for serial to stablize

  WiFi.mode(WIFI_STA); //setting wifi in station mode //putting wifi radio in station mode which is required for espnow. we are not connecting to any router here. this enables STA interface

  // initlazing the ESPNOW on the uav esp after the wifi is set to station mode
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    while(true) delay(1000);
  }

  //now register the callback function so we can see if each send is successful or not
  esp_now_register_send_cb(afterDataSent);

  // now we must add the reciever as a peer in the espnow before sendding.
  // a peer is basically a known MAC address target that ESPNow can send to. aka SSN of that target ESP32 (UGV)
  esp_now_peer_info_t peerInfo = {}; // Initialize with default values
  
  memcpy(peerInfo.peer_addr, UGV_MAC, 6); // copy the reciever UGV MAC address into the peer struct
  peerInfo.channel = 0; // setting 0 means telling it to use current wifi channel
                        // since we arenot connect to Wifi , channel 0 usually works
  peerInfo.encrypt = false; // disabling encryption for now for simplicity
  

  // now finally adding the peer into the ESP-Now peer list
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  
  Serial.print("Sender ready. My MAC is: ");
  Serial.println(WiFi.macAddress());
}

void loop()
{
  // the payload curenntly m sending is very basic simple null termineted string
  // but in future we can send more complex data like velocuity commands
  // for now we are just sending a simple string
  const char *message = "Hello from UAV! how are you UGV?";


  // esp_now_send() is the function that sends the data
  // it takes the peer MAC address, the data pointer, and the data length
  //so esp_now_send(target_mac, data_pointer, data_length)
  // i also included +1 so the reciever gets the '\0' terminator and can safelly print as a stirng
  esp_err_t result = esp_now_send(UGV_MAC, 
                                  (const uint8_t *)message, strlen(message) + 1);

  // result is ESP_OK if the radio accepted the packet for transmission
  // it does NOT guarantee delivery to the receiver
  // for guaranteed delivery, we would need acks from the receiver


  if ((int)result == 0)
  {
    Serial.println("got yr message. let me try to deliver it");
  }
  else
  {
    Serial.println("couldnt get yr message. check the mac address or the message size making sure within limit of 250 ");
  }
  delay(5000); // wait for 1 second before sending the next message
}
