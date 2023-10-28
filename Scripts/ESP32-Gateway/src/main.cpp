#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <NeoPixelBus.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t newMACAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

#define RELAY_PIN 6

const uint16_t PixelCount = 3; 
const uint8_t PixelPin = 5; 

#define colorSaturation 128
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

#define MY_TABLE_NUM 2 // Define this units table num, Maybe need to make it ask on reset
boolean active = false;

//7C:DF:A1:FD:70:D4
uint8_t master_address[] = {0x7C,0xDF,0xA1, 0xFD,0x70,0xD4};
esp_now_peer_info_t peer;

HardwareSerial RFID(1);
const int BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_SIZE = 10; // 10byte data (2byte version + 8byte tag)
const int DATA_VERSION_SIZE = 2; // 2byte version (actual meaning of these two bytes may vary)
const int DATA_TAG_SIZE = 8; // 8byte tag
const int CHECKSUM_SIZE = 2; // 2byte checksum
char tag_buffer [8];
//SoftwareSerial ssrfid = SoftwareSerial(7,8, false, 256); 

uint8_t buffer[BUFFER_SIZE]; // used to store an incoming data frame 
int buffer_index = 0;

int test = 0;

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
StaticJsonDocument<192> doc;


struct data {
  u_int8_t TABLE_NUM;
  int STATUS;
  //u_int8_t STATUS;
  int timeIn;
};

struct Stat{
  boolean b;
  boolean e;
  boolean r;
  boolean a;
};


data newdata;
Stat current_stat;


void allow_access(){
  strip.SetPixelColor(0, black);
  strip.Show();
  active = true;
  digitalWrite(RELAY_PIN, HIGH);
  active = 1;
}

void deny_access(){

  strip.SetPixelColor(0, red);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, blue);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, red);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, blue);
  strip.Show();
  delay(200);

  digitalWrite(RELAY_PIN, LOW);

  active = false;
}

void handle_status(){
  // Now we need to manage the status

  if(current_stat.b == 0 && current_stat.e == 0 && current_stat.r == 0 && current_stat.a == 0){
    // Time to reset
    digitalWrite(RELAY_PIN, LOW);
  }
  else{
      // First check if response flag is high
    if (current_stat.r == 1){
      // Master is responding to request, ignore rest and just accept or don't accept
      if (current_stat.a == 1){
        // Allow access:
        allow_access();

      }
      else{
        // Dont allow access:
        deny_access();
      }
    }
    else{
      // Message must be a routine status update
      if(current_stat.b and active == false){
        strip.SetPixelColor(0, blue);
        strip.Show();
      }

      if(current_stat.e){
        strip.SetPixelColor(0, green);
        strip.Show();
        delay(1000);
        strip.SetPixelColor(0, black);
        strip.Show();
        delay(1000);
        strip.SetPixelColor(0, green);
        strip.Show();
        delay(1000);
        strip.SetPixelColor(0, black);
        strip.Show();
        delay(1000);
        strip.SetPixelColor(0, green);
        strip.Show();
        delay(1000);
        strip.SetPixelColor(0, black);
        strip.Show();
      }
    }
  }
  
  
  Serial.println("Status managed");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *data, int len) {
  
  memcpy(&newdata, data, len);
  Serial.printf("Received Data: Table = %d, status = %d, Size = %d, Time = %d \n", newdata.TABLE_NUM, newdata.STATUS, len, newdata.timeIn);
  if(newdata.TABLE_NUM == 0){
    StaticJsonDocument<64> doc;
    doc["t"] = 0;
    doc["s"] = newdata.STATUS;
    int l = test;
    doc["l"] = l;
    doc["time"] = newdata.timeIn;

    
    String j = "";
    serializeJson(doc, j);

    uint8_t *buffer = (uint8_t*) j.c_str();
    size_t sizeBuff = sizeof(buffer) * j.length();
    esp_now_send(master_address, buffer, sizeBuff);
    test +=1;
  }
  if(newdata.TABLE_NUM == MY_TABLE_NUM){
    std::array<int, 4> result = decodeStatus(newdata.STATUS);
    Serial.print(result[0]); Serial.print(" ");
    Serial.print(result[1]); Serial.print(" ");
    Serial.print(result[2]); Serial.print(" ");
    Serial.print(result[3]); Serial.print(" ");

    // Update status then go manage
    current_stat.b = result[0];
    current_stat.e = result[1];
    current_stat.r = result[2];
    current_stat.a = result[3];
    handle_status();
  }
}

void setup(){
    //Initialize Serial Monitor
  Serial.begin(115200);
  RFID.begin(9600, SERIAL_8N1, 18, 17);

  pinMode(RELAY_PIN, OUTPUT);
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  
  Serial.print("[NEW] ESP32 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  strip.Begin();
  strip.Show();

  memcpy(peer.peer_addr, newMACAddress, 6);
  peer.channel = 0;  
  peer.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peer) != ESP_OK){
    Serial.println("Failed to add peer");
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

  delay(3000);
}

void extract_tag() {
    uint8_t msg_head = buffer[0];
    uint8_t *msg_data = buffer + 1; // 10 byte => data contains 2byte version + 8byte tag
    uint8_t *msg_data_version = msg_data;
    uint8_t *msg_data_tag = msg_data + 2;
    uint8_t *msg_checksum = buffer + 11; // 2 byte
    uint8_t msg_tail = buffer[13];

    // print message that was sent from RDM630/RDM6300
    Serial.println("--------");

    Serial.print("Message-Head: ");
    Serial.println(msg_head);

    Serial.println("Message-Data (HEX): ");
    for (int i = 0; i < DATA_VERSION_SIZE; ++i) {
      Serial.print(char(msg_data_version[i]));
  
    }
    Serial.println(" (version)");
    for (int i = 0; i < DATA_TAG_SIZE; ++i) {
      Serial.print(char(msg_data_tag[i]));
      tag_buffer[i] = msg_data_tag[i];
    }
    Serial.println(" (tag)");

    Serial.print("Message-Checksum (HEX): ");
    for (int i = 0; i < CHECKSUM_SIZE; ++i) {
      Serial.print(char(msg_checksum[i]));
    }
    Serial.println("");

    Serial.print("Message-Tail: ");
    Serial.println(msg_tail);

    Serial.println("--");
}
 
void loop(){
  if (RFID.available()){
    bool call_extract_tag = false;
    String outstr = "";
    int ssvalue = RFID.read(); // read 
    if (ssvalue == -1) { // no data was read
      return;
    }
  
  
    if (ssvalue == 2) { // RDM630/RDM6300 found a tag => tag incoming 
      buffer_index = 0;
    } 
    else if (ssvalue == 3) { // tag has been fully transmitted       
      call_extract_tag = true; // extract tag at the end of the function call
    }
    buffer[buffer_index++] = ssvalue; // everything is alright => copy current value to buffer



    if (call_extract_tag == true) {
      if (buffer_index == BUFFER_SIZE) {
        extract_tag();

        // Time to send tag
        Serial.println("Going to send:");
        for (int i = 0; i != 8; i++){
          Serial.print(tag_buffer[i]);
          outstr = outstr + tag_buffer[i];
        }

        StaticJsonDocument<64> doc;
        doc["t"] = MY_TABLE_NUM;
        doc["s"] = outstr;
        String output = "";
        serializeJson(doc, output);

        uint8_t *buffer = (uint8_t*) output.c_str();
        size_t sizeBuff = sizeof(buffer) * outstr.length();
        esp_now_send(master_address, buffer, sizeBuff);
        delay(1000);
        Serial.println("Clearing Buffer...");
        // Basically keep reading till -1 or else cards stack
        int temp = 1;
        while(temp != -1){
          temp = RFID.read();
        }
      } 
      else { // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        Serial.println("Something went wrong");
        return;
      }
      
    }
  }


  
    
}
