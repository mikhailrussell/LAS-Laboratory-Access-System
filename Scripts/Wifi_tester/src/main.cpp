#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>
#include "painlessmesh/protocol.hpp"
#include "plugin/performance.hpp"

Scheduler     userScheduler; // to control your personal task
using namespace painlessmesh;
#define   MESH_SSID       "white-lab-mesh"
#define   MESH_PASSWORD   "justinhelp"
#define   MESH_PORT       5555
painlessMesh  mesh;
StaticJsonDocument<192> doc;
int test = 0;
int buf[200];
struct table_0_data{
  int table;
  int s;
  int l;
  int RSSI;
};
table_0_data data;
SimpleList<uint32_t> nodes;
uint8_t master_address[] = {0x7C,0xDF,0xA1, 0xFD,0x70,0xD4};

// typedef struct {
//     uint8_t bssid[6];                     /**< MAC address of AP */
//     uint8_t ssid[33];                     /**< SSID of AP */
//     uint8_t primary;                      /**< channel of AP */
//     wifi_second_chan_t second;            /**< secondary channel of AP */
//     int8_t  rssi;                         /**< signal strength of AP */
//     wifi_auth_mode_t authmode;            /**< authmode of AP */
//     wifi_cipher_type_t pairwise_cipher;   /**< pairwise cipher of AP */
//     wifi_cipher_type_t group_cipher;      /**< group cipher of AP */
//     wifi_ant_t ant;                       /**< antenna used to receive beacon from AP */
//     uint32_t phy_11b:1;                   /**< bit: 0 flag to identify if 11b mode is enabled or not */
//     uint32_t phy_11g:1;                   /**< bit: 1 flag to identify if 11g mode is enabled or not */
//     uint32_t phy_11n:1;                   /**< bit: 2 flag to identify if 11n mode is enabled or not */
//     uint32_t phy_lr:1;                    /**< bit: 3 flag to identify if low rate is enabled or not */
//     uint32_t wps:1;                       /**< bit: 4 flag to identify if WPS is supported or not */
//     uint32_t ftm_responder:1;             /**< bit: 5 flag to identify if FTM is supported in responder mode */
//     uint32_t ftm_initiator:1;             /**< bit: 6 flag to identify if FTM is supported in initiator mode */
//     uint32_t reserved:25;                 /**< bit: 7..31 reserved */
//     wifi_country_t country;               /**< country information of AP */
// } wifi_ap_record_t;

wifi_ap_record_t record;
typedef struct {
  unsigned frame_ctrl: 16;
  unsigned duration_id: 16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl: 16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

bool first = true;
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {

    // All espnow traffic uses action frames which are a subtype of the mgmnt frames so filter out everything else.
    // if (type != WIFI_PKT_MGMT)
    //   return;

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
    
    int rssi = ppkt->rx_ctrl.rssi;
    uint8_t mac[6];
    memcpy(mac, hdr->addr2, 6);
    // for(int i = 0; i < 6; i++){
    //   Serial.printf("%x",mac[i]);
    //   Serial.print(",");
    // }
    if (memcmp(mac, master_address, 6) == 0) {
    data.RSSI = rssi;
    first = false;
    }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&buf, incomingData, sizeof(incomingData));
}
void receivedCallback(uint32_t from, String & msg) {
  
  Serial.printf("%s\n", msg.c_str());
  String modifiedMsg = msg;
  modifiedMsg.replace("'", "\"");
  DeserializationError error = deserializeJson(doc,msg.c_str());
  int t = doc["t"].as<int>(); // 1
  int s = doc["s"].as<int>(); // 12
  int time = doc["time"];
  int l = test;

  String output = "";
  doc["t"] = 0;
  doc["s"] = s;
  doc["l"] = l;
  doc["time"] = time;
  doc["rssi"] = data.RSSI;
  
  for(int i = 0; i<33;i++){
    char ssid_char = record.ssid[i];
    Serial.print(ssid_char);
  }
  Serial.println();

  serializeJson(doc, output);
  mesh.sendBroadcast(output);
  test = test+1;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}
void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages
  
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  plugin::performance::begin(mesh, 2);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);


  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);

  Serial.println("Setup done");
}

void loop() {
  mesh.update();
  esp_wifi_sta_get_ap_info(&record);
  //esp_wifi_sta_get_ap_info(&record);
  //esp_wifi_scan_get_ap_records
  // WiFi.scanNetworks will return the number of networks found
  // int n = WiFi.scanNetworks();
  // if (n == 0) {
  //     Serial.println("no networks found");
  // } else {
  //   for (int i = 0; i < n; ++i) {
  //     // Print SSID and RSSI for each network found
  //     if(WiFi.SSID(i) == "white-lab-mesh"){
  //       Serial.println(WiFi.RSSI(i));
  //     }
  //   }
  // }

  // Wait a bit before scanning again
}
