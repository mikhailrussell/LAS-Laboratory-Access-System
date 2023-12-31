//************************************************************
// this is a simple example that uses the easyMesh library
//
// 1. blinks led once for every node on the mesh
// 2. blink cycle repeats every BLINK_PERIOD
// 3. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 4. prints anything it receives to Serial.print
//
//
//************************************************************
#include <painlessMesh.h>
#include <HardwareSerial.h>
#include "painlessmesh/protocol.hpp"
#include "plugin/performance.hpp"


using namespace painlessmesh;


#define   MESH_SSID       "white-lab-mesh"
#define   MESH_PASSWORD   "justinhelp"
#define   MESH_PORT       5555

// Prototypes
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;


bool calc_delay = false;
SimpleList<uint32_t> nodes;

void RPI_recieve();
#define PERIOD1 40
Task writetopi(PERIOD1 * TASK_MILLISECOND, TASK_FOREVER ,&RPI_recieve, &userScheduler);
bool onFlag = false;

HardwareSerial RPI(1);
String inputString = "";

void RPI_recieve(void){
  if(RPI.available()){
    while (RPI.available()) {

      char c = RPI.read();
      if (c == '\n'){
        Serial.println(inputString);
        mesh.sendBroadcast(inputString);
        inputString = "";
      }
      else{
        inputString += c;
      }
      
      
    }
    
  }
  //Serial.printf("Sending: %s \n", inputString);
 
 
}


void setup() {
  Serial.begin(115200);
  RPI.setRxBufferSize(4086);
  RPI.setTxBufferSize(4086);
  RPI.begin(115200, SERIAL_8N1, 18, 17);
  //RPI.onReceive(&RPI_recieve);
  
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  plugin::performance::begin(mesh, 2);
  mesh.onReceive(&receivedCallback);
  userScheduler.addTask(writetopi);
  writetopi.enable();
 
  //mesh.onNewConnection(&newConnectionCallback);
  // mesh.onChangedConnections(&changedConnectionCallback);
  //mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  //mesh.onNodeDelayReceived(&delayReceivedCallback);
  
}

void loop() {
  mesh.update();
}

void receivedCallback(uint32_t from, String & msg) {
  RPI.println(msg.c_str());
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

void serialEvent() {
  String inputString = "";
  
  while (Serial.available()) {

    char c = Serial.read();
    inputString += c;
  }
  mesh.sendBroadcast(inputString);
}
