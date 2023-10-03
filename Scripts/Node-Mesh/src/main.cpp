//************************************************************
//
//************************************************************
#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <NeoPixelBus.h>

#define MY_TABLE_NUM 1 // Define this units table num, Maybe need to make it ask on reset

#define   MESH_SSID       "white-lab-mesh"
#define   MESH_PASSWORD   "justinhelp"
#define   MESH_PORT       5555

#define RELAY_PIN 6

const uint16_t PixelCount = 2; 
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


boolean active = false;

// Prototypes
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

StaticJsonDocument<192> doc;

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;

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

std::array<int, 4> UpdateStatus(int status) {
    std::array<int, 4> ret;
    ret[0] = (status >> 3) & 1; // (b)
    ret[1] = (status >> 2) & 1; // (e)
    ret[2] = (status >> 1) & 1; // (r)
    ret[3] = status & 1;        // (a)
    return ret;
}

int test = 0;
// Json stuff
String incoming_string;


struct data {
  u_int8_t TABLE_NUM;
  u_int8_t STATUS;
  u_int8_t LED_colour;
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
  Serial.println("Allowing Access");
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

void setup() {
  Serial.begin(115200);
  RFID.begin(9600, SERIAL_8N1, 18, 17);

  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  // mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  // mesh.onNodeDelayReceived(&delayReceivedCallback);

  strip.Begin();
  strip.Show();

  Serial.println("Setup Complete!");
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

void loop() {
  mesh.update();

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

        doc["t"] = MY_TABLE_NUM;
        doc["s"] = outstr;
        doc["id"] = mesh.getNodeId();

        String output = ""; 
        serializeJson(doc,output);
        mesh.sendBroadcast(output);
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

void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("%s\n", msg.c_str());
  String modifiedMsg = msg;
  modifiedMsg.replace("'", "\"");
  DeserializationError error = deserializeJson(doc,msg.c_str());
  if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
  }
  int t = doc["t"].as<int>(); // 1
  int s = doc["s"].as<int>(); // 12
  int time = doc["time"];
  int l = test;
  

  //Serial.print(t);
  //Serial.println(s);
  //int l = doc["l"]; // 255
  newdata.TABLE_NUM = t;
  newdata.STATUS =s;
  if(newdata.TABLE_NUM == 0 and test != 10000){
    String output = "";
    doc["t"] = 0;
    doc["s"] = s;
    int l = test;
    doc["l"] = l;
    doc["time"] = time;

    serializeJson(doc, output);
    mesh.sendBroadcast(output);
    test = test+1;
  }
  if(newdata.TABLE_NUM == MY_TABLE_NUM){
    Serial.print(t);  
    Serial.println(s);
    std::array<int, 4> result = UpdateStatus(newdata.STATUS);
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

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}