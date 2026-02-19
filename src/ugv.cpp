#include <Arduino.h> //basic arduino functions (serial, delay, setup/loop)
#include <WiFi.h> // controls ESP32 wifi raduo mode to STA or AP mode. espnow uses this radio
#include <esp_now.h> // espno2 api. it has init, send/recieve callbacks, peer management

// this callback runs automatically whenver an ESPNOW packet is recieved
static void whenDataIsRecieved(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    //mac = sender's mac address as 6 raw bytes (not a string)
    // incomingData = pointer to recieved bytes
    // len = number of recieved bytes

    // printing the sender mac address in human readable format
    Serial.printf("Recieved %d bytes from MAC: %02X:%02X:%02X:%02X:%02X:%02X | ", 
        len, //printing the length of the packet
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] //printing the mac address
     );


    //for step 2, we are sending a simple null terminted string
    //so we can copy the incoming data into a buffer and print it as a string 
    //so we copy into a local buffer and force '\0' at the end to safely print it.
    char localMemoryBuffer[250]; //local buffer to store the incoming data  //getting piece of empty paper that can hold 250 characters
    int n = (len <240) ? len : 249; //keep room for null terminator
    memcpy(localMemoryBuffer, incomingData, n); //actually copying n bytes from incomingData into buffer
    localMemoryBuffer[n] = '\0'; //forcing null terminator at the end
    Serial.println(localMemoryBuffer); //printing the recieved message from UAV
}

void setup()
{
    Serial.begin(115200); //start serial logging at 115200 baud
    delay(500); //wait for serial to stabilize

    //setting the ESP32 into station mode
    WiFi.mode(WIFI_STA); //setting wifi in station mode 
                        //putting wifi radio in station mode which is required for espnow. we are not connecting to any router here. this enables STA interface
    
    
    // initializing ESP-NOW subsystem
    // if this fails, nothing else will work
    if (esp_now_init() != ESP_OK) // esp_now_init() returns ESP_OK if successful // esp_ok means success
    {
        Serial.println("Error initializing ESP-NOW");
        while(true) delay(1000); //halt execution if esp-now fails //stop the code from running further
    }

    /*
    esp-now Register [a] Recieve Callback
    aka
    hey respnow, please sign up this function to be called back whenever we recie a message
    */
    // register the callback function that will be executed when data is recieved
    esp_now_register_recv_cb(whenDataIsRecieved); //registering the callback function //this function will be called automatically whenever data is recieved
    // now the ESP32 will call whenDataIsRecieved() automatically whenever it recieves an ESPNOW packet

    // print reciever status + our own mac 
    Serial.println("Hi, I am UGV! Reciever ready!");
    Serial.print("MAC Address of the ESP32: ");
    Serial.println(WiFi.macAddress());
}

void loop()
{
    // reciver is event-driven: it waits for packets and whenDataIsRecieved() prints them
    // so loop can be empty or do other tasks
    delay(1000); // idle loop delay (keeps CPU calm)
}

