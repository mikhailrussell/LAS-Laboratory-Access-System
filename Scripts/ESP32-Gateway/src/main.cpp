#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <NeoPixelBus.h>

const uint16_t PixelCount = 1; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 48;  // make sure to set this to the correct pin, ignored for Esp8266

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

#define MY_TABLE_NUM 1 // Define this units table num, Maybe need to make it ask on reset
boolean active = false;
uint8_t master_address[] = { 0xC4,0xDE,0xE2,0x12,0x4E,0xD4};
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

void allow_access(){
  strip.SetPixelColor(0, green);
  strip.Show();
  active = true;
}

void deny_access(){
  strip.SetPixelColor(0, red);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, black);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, red);
  strip.Show();
  delay(200);
  strip.SetPixelColor(0, black);
  strip.Show();
  delay(200);

  active = false;
}

void manage(){
  // Now we need to manage the status
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
    else if (active == false){
      strip.SetPixelColor(0, white);
      strip.Show();
    }

    if(current_stat.e){
      strip.SetPixelColor(0, green);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, red);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, green);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, red);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, green);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, red);
      strip.Show();
      delay(1000);
      strip.SetPixelColor(0, green);
      strip.Show();
      delay(1000);
    }
  }
  
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
  strip.Begin();
  strip.Show();

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

StaticJsonDocument<64> doc;

doc["t"] = MY_TABLE_NUM;
doc["s"] = "RSSMIK001";
String output = "";
serializeJson(doc, output);

uint8_t *buffer = (uint8_t*) output.c_str();
size_t sizeBuff = sizeof(buffer) * output.length();
esp_now_send(master_address, buffer, sizeBuff);

Serial.print("Sent: ");
Serial.println(output);
}
 
void loop(){

}
