#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define MY_TABLE_NUM 1 // Define this units table num, Maybe need to make it ask on reset

uint8_t master_address[] = { 0x7C,0xDF,0xA1, 0xFD, 0x70, 0xD4};
esp_now_peer_info_t peer;

std::array<int, 4> decodeStatus(int status) {
    std::array<int, 4> values;
    values[0] = (status >> 3) & 1; // Extract bit 3 (b)
    values[1] = (status >> 2) & 1; // Extract bit 2 (e)
    values[2] = (status >> 1) & 1; // Extract bit 1 (r)
    values[3] = status & 1;        // Extract bit 0 (a)
    return values;
}


// Json stuff
String incoming_string;
DynamicJsonDocument doc(1024);

struct data {
  int TABLE_NUM;
  int STATUS;
};

struct Stat{
  boolean b;
  boolean e;
  boolean r;
  boolean a;
};

data newdata;
Stat current_stat;

void manage(){
  Serial.println("Status managed");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *data, int len) {
  // byte bitTrain = data[0]; // Received bitTrain
  // Serial.println("Received Bit Train: " + String(bitTrain, BIN));

  // Copy the received data into the struct
  memcpy(&newdata, data, len);
  Serial.printf("Received Data: Table = %d, status = %d, Size = %d\n", newdata.TABLE_NUM, newdata.STATUS, len);

  std::array<int, 4> result = decodeStatus(newdata.STATUS);
  Serial.print(result[0]); Serial.print(" ");
  Serial.print(result[1]); Serial.print(" ");
  Serial.print(result[2]); Serial.print(" ");
  Serial.print(result[3]); Serial.print(" ");

  if(newdata.TABLE_NUM == MY_TABLE_NUM){
    // Update status then go manage
    current_stat.b = result[0];
    current_stat.e = result[1];
    current_stat.r = result[2];
    current_stat.a = result[3];
    manage();
  }
  

}

void setup(){
    //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peer.peer_addr, master_address, 6);
  peer.channel = 0;  
  peer.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peer) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop(){

delay(3000);

StaticJsonDocument<64> doc;

doc["t"] = 1;
doc["s"] = "RSSMIK001";
String output = "";
serializeJson(doc, output);

uint8_t *buffer = (uint8_t*) output.c_str();
size_t sizeBuff = sizeof(buffer) * output.length();
esp_now_send(master_address, buffer, sizeBuff);

Serial.print("Sent: ");
Serial.println(output);

}