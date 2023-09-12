#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

String incoming_string;
DynamicJsonDocument doc(1024);

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char* in = (char*) incomingData;
  String datastr = String(in);
  Serial.println(datastr);

  DeserializationError error = deserializeJson(doc, datastr);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int TABLE_NUM = doc["TABLE_NUM"]; // 2
  String STDNUM = doc["STDNUM"]; // "RSSMIK001"

  Serial.print("Student Number:");
  Serial.println(STDNUM);
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
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop(){

}