#include <Preferences.h>
#include "BluetoothSerial.h"


// wifi
String ssids_array[50];
String network_string;
String connected_string;
const char* pref_ssid = "";
const char* pref_pass = "";
String client_wifi_ssid;
String client_wifi_password;
long start_wifi_millis;
long wifi_timeout = 10000;
enum wifi_setup_stages { NONE, SCAN_START, SCAN_COMPLETE, SSID_ENTERED, WAIT_PASS, PASS_ENTERED, WAIT_CONNECT, LOGIN_FAILED, CONNECTED };
enum wifi_setup_stages wifi_stage = NONE;
Preferences preferences;

// bluetooth
const char* bluetooth_name = "robot01";
bool bluetooth_disconnect = false;
BluetoothSerial SerialBT;

/*
* function : scan wifi
* return : none
*/
void scan_wifi_networks()
{
  WiFi.mode(WIFI_STA);
  // WiFi.scanNetworks will return the number of networks found
  int n =  WiFi.scanNetworks();
  if (n == 0) {
    SerialBT.println("no networks found");
  } else {
    SerialBT.println();
    SerialBT.print(n);
    SerialBT.println(" networks found");
    delay(1000);
    for (int i = 0; i < n; ++i) {
      ssids_array[i + 1] = WiFi.SSID(i);
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(ssids_array[i + 1]);
      network_string = i + 1;
      network_string = network_string + ": " + WiFi.SSID(i) + " (Strength:" + WiFi.RSSI(i) + ")";
      SerialBT.println(network_string);
    }
    wifi_stage = SCAN_COMPLETE;
  }
}

/*
* function : get the wifi id and password from the phone
* return : none
*/
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    wifi_stage = SCAN_START;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == SCAN_COMPLETE) { // data from phone is SSID
    int client_wifi_ssid_id = SerialBT.readString().toInt();
    client_wifi_ssid = ssids_array[client_wifi_ssid_id];
    wifi_stage = SSID_ENTERED;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == WAIT_PASS) { // data from phone is password
    client_wifi_password = SerialBT.readString();
    client_wifi_password.trim();
    wifi_stage = PASS_ENTERED;
  }
}

/*
* function : print the IP after connecting to the wifi
* return : none
*/
void callback_show_ip(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    SerialBT.print("ESP32 IP: ");
    SerialBT.println(WiFi.localIP());
    bluetooth_disconnect = true;
  }
}

/*
* function : disconnect bluetooth module - after connect wifi sucess
* return : none
*/
void disconnect_bluetooth()
{
  delay(1000);
  Serial.println("BT stopping");
  SerialBT.println("Bluetooth disconnecting...");
  delay(1000);
  SerialBT.flush();
  SerialBT.disconnect();
  SerialBT.end();
  Serial.println("BT stopped");
  delay(1000);
  bluetooth_disconnect = false;
}

/*
* function : initial wifi module
* return : none
*/
bool init_wifi()
{
  String temp_pref_ssid = preferences.getString("pref_ssid");
  String temp_pref_pass = preferences.getString("pref_pass");
  pref_ssid = temp_pref_ssid.c_str();
  pref_pass = temp_pref_pass.c_str();

  Serial.println(pref_ssid);
  Serial.println(pref_pass);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);

  start_wifi_millis = millis();
  WiFi.begin(pref_ssid, pref_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_wifi_millis > wifi_timeout) {
      WiFi.disconnect(true, true);
      return false;
    }
  }
  return true;
}

/*
* function : setup for wifi module
* if the previously connected wifi is not available 
* then using the bluetooth module to setup
* return : none
*/
void setupWifi()
{
    preferences.begin("wifi_access", false);

  if (!init_wifi()) { 
    // Auto connect to Wi-Fi fail
    // using the bluetooth to setup wifi
    Serial.println(" Connect to Wi-Fi fail");
    // using serial bluetooth to communicate to phone
    SerialBT.register_callback(callback);
  } else {
    // connect to Wi-Fi sucess
    Serial.println(" Connect to Wi-Fi sucess");
    wifi_stage = CONNECTED;
    SerialBT.register_callback(callback_show_ip);
  }

  SerialBT.begin(bluetooth_name);
}

/*
* function : scanning the wifi networks
* return : none
*/
void wifiScan()
{
    Serial.println("Scanning --------");
    Serial.println(wifi_stage);
    switch (wifi_stage)
    {
        case SCAN_START:
        SerialBT.println("Scanning Wi-Fi networks");
        Serial.println("Scanning Wi-Fi networks");
        scan_wifi_networks();
        SerialBT.println("Please enter the number for your Wi-Fi");
        wifi_stage = SCAN_COMPLETE;
        break;

        case SSID_ENTERED:
        SerialBT.println("Please enter your Wi-Fi password");
        Serial.println("Please enter your Wi-Fi password");
        wifi_stage = WAIT_PASS;
        break;

        case PASS_ENTERED:
        SerialBT.println("Please wait for Wi-Fi connection...");
        Serial.println("Please wait for Wi_Fi connection...");
        wifi_stage = WAIT_CONNECT;
        preferences.putString("pref_ssid", client_wifi_ssid);
        preferences.putString("pref_pass", client_wifi_password);
        if (init_wifi()) { // Connected to WiFi
            connected_string = "ESP32 IP: ";
            connected_string = connected_string + WiFi.localIP().toString();
            SerialBT.println(connected_string);
            Serial.println(connected_string);
            bluetooth_disconnect = true;
        } else { // try again
            wifi_stage = LOGIN_FAILED;
        }
        break;

        case LOGIN_FAILED:
        SerialBT.println("Wi-Fi connection failed");
        Serial.println("Wi-Fi connection failed");
        delay(2000);
        wifi_stage = SCAN_START;
        break;

        default:
        break;
    }

    //when the wifi is connected then the bluetooth has to disconnect
    if (bluetooth_disconnect)
    {
        disconnect_bluetooth();
    }
}

//
int setUpCycleTime()
{
    int time = 0;


    return time;
}
