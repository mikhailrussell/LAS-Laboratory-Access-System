#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <WiFi.h>
//json stuff
DynamicJsonDocument doc(1024);

esp_now_peer_info_t peer;
// C4:DE:E2:12:4E:D4
uint8_t broadcastAddress[] = { 0xC4,0xDE,0xE2,0x12,0x4E,0xD4};

char string[200];
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  char* in = (char*) incomingData;
  String datastr = String(in);
  Serial.println(datastr);

}

void setup() {
  
  Serial.begin(115200);
  Serial.println("Begin Setup...");
  
  // Init wifi in station mode
  WiFi.mode(WIFI_STA);
  Serial.println("Wifi Started...");
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW Initilised");

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  Serial.println("Begin adding peer...");
  // Register peer
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = 0;  
  peer.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peer) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Setup Completed");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "1" : "0");
}

void loop() {

}

/*
  SerialEvent called when Nodered tries to send data.
*/
void serialEvent() {
  
  char incomingChars[5];
  byte bitTrain = 0;

  while (Serial.available()) {

    for (int i = 0; i < 5; i++) {
      incomingChars[i] = Serial.read();
    }

    for (int i = 0; i < 5; i++) {
      if (incomingChars[i] == '1') {
        bitTrain |= (1 << (4 - i)); // Set the corresponding bit to 1
      } else if (incomingChars[i] == '0') {
        // Do nothing, the bit is already 0
      }
    }
  }

  //memcpy(&string, inputString.c_str(), strlen(inputString.c_str()));
  //uint8_t *buffer = (uint8_t*) inputString.c_str();
  int result = esp_now_send(broadcastAddress, &bitTrain, sizeof(bitTrain));
  if (result != ESP_OK){
    //Serial.print("Tried to send: ");
    //Serial.println(inputString);
    //Serial.print("Size: ");
    //Serial.println(sizeof(buffer)*strlen(inputString.c_str()));
  }
}