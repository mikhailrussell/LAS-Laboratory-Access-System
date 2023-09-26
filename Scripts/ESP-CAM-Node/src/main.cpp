#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <WiFi.h>
//json stuff
DynamicJsonDocument doc(1024);

esp_now_peer_info_t peer;
// C4:DE:E2:12:4E:D4
uint8_t broadcastAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

char string[200];
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

struct data {
  // try char int
  int TABLE_NUM;
  int STATUS;
};

data newdata;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  // char* in = (char*) incomingData;
  // String datastr = String(in);
  // Serial.println(datastr);

  // Copy the received data into the struct
  // memcpy(&newdata, incomingData, sizeof(incomingData));
  // Serial.printf("Received Data: Table = %d, status = %d\n", newdata.TABLE_NUM, newdata.STATUS);

  char* buff = (char*) incomingData;
  String buffStr = String(buff);
  Serial.println(buffStr);

  

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
// void serialEvent() {
  
//   byte bytebuffer[10];

//   char string[200];
//   String inputString = "";      // a String to hold incoming data
//   bool stringComplete = false;  // whether the string is complete
//   byte inbyte;
//   int table;
//   int stat;

//   int i = 0;
//   while (Serial.available()) {

//     char c = Serial.read();

//     if (c == ',') {
//       // Found a comma delimiter, parse the values
//       newdata.TABLE_NUM = inputString.toInt();
//       inputString = ""; // Clear the input string
//     } 
//     else if (c == '\n') {
//       // Found a newline, which indicates the end of the data
//       newdata.STATUS = inputString.toInt();
//       }

//     // table = Serial.parseInt();
//     // stat = Serial.parseInt();

//     // // Create a data structure to hold the values
//     // newdata.TABLE_NUM = table;
//     // newdata.STATUS = stat;

    
//     // inbyte = Serial.read();
//     // bytebuffer[i] = inbyte;
//     // i++;
//     // Serial.print(inbyte); Serial.print(' ');
//     //inputString = inputString + Serial.read(); 

//     // Serial.println(*incomingint);

//     // for (int i = 0; i < 3; i++) {
//     //   if (incomingChars[i] == '1') {
//     //     bitTrain |= (1 << (4 - i)); // Set the corresponding bit to 1
//     //   } else if (incomingChars[i] == '0') {
//     //     // Do nothing, the bit is already 0
//     //   }
//     // }

//   }
//   delay(100);
//   Serial.printf("Received Data: Table = %d, status = %d\n", newdata.TABLE_NUM, newdata.STATUS);

//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&newdata, sizeof(data));

//     if (result == ESP_OK) {
//       Serial.println("Data sent successfully");
//     } else {
//       Serial.println("Error sending data");
//     }
//   // Serial.println();

//   // for (int i = 0; i < 10; i++) {
//   //   Serial.print(bytebuffer[i], BIN);
//   //   Serial.print(' ');
//   // }
//   // Serial.println();
//   // memcpy(&string, inputString.c_str(), strlen(inputString.c_str()));
//   // uint8_t *buffer = (uint8_t*) inputString.c_str();
//   // int result = esp_now_send(broadcastAddress, bytebuffer, 4);
//   //if (result != ESP_OK){
//     //Serial.print("Tried to send: ");
//     //Serial.println(inputString);
//     //Serial.print("Size: ");
//     //Serial.println(sizeof(buffer)*strlen(inputString.c_str()));
//   //}
// }

void serialEvent() {
  static String inputString = "";
  
  while (Serial.available()) {
    char c = Serial.read();

    if (c == ',') {
      // Found a comma delimiter, parse the values
      newdata.TABLE_NUM = inputString.toInt();
      inputString = ""; // Clear the input string
    } else if (c == '&') {
      // Found a newline, which indicates the end of the data
      newdata.STATUS = inputString.toInt();

      // Transmit the data structure using ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&newdata, sizeof(data));

      // Clear the input string for the next data
      inputString = "";
    } else {
      // Add the character to the input string
      inputString += c;
    }
  }
}
