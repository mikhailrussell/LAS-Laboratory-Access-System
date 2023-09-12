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

void setup() {
  
  Serial.begin(115200);
  Serial.println("Begin Setup...");
  
  // Init wifi in station mode
  WiFi.mode(WIFI_STA);
  Serial.println("Wifi Started...");

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

  Serial.println("Setup Completed");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "1" : "0");
}

void loop() {

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  inputString = "";

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

  }

  memcpy(&string, inputString.c_str(), strlen(inputString.c_str()));
  uint8_t *buffer = (uint8_t*) inputString.c_str();
  int result = esp_now_send(broadcastAddress, buffer, sizeof(buffer)*strlen(inputString.c_str()));
  // Serial.print("Sent: ");
  // Serial.print(string);
  // Serial.print(" Size: ");
  // Serial.println(sizeof(string));
  
  // DeserializationError error = deserializeJson(doc, inputString);

  // if (error) {
  //   Serial.print("deserializeJson() failed: ");
  //   Serial.println(error.c_str());
  //   return;
  // }

  // int TABLE_NUM = doc["TABLE_NUM"]; // 2
  // String STDNUM = doc["STDNUM"]; // "RSSMIK001"

  // Serial.print("Student Number:");
  // Serial.println(STDNUM);
}