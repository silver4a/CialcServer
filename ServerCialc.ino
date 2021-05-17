#include <WiFi.h>
#include <Preferences.h>
//Bluetooth.
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

const char* ssid     = "RODO";
const char* password = "32828526D";
const char* bluetooth_name = "CIALC_DEVICE";
//Comentario pruebas
bool bluetooth_state = false;

//-----------------------------------
int progresoIntensidad = 0;
int progresoTemperatura = 0;
//-----------------------------------

String host = "";
int ipStatic[4];
int subnetStatic[4];
int gatewayStatic[4];
//---------------------------------------------------
//WiFi global variable.
int start_wifi_millis;
int wifi_timeout = 10000;
enum wifi_setup_stages { NONE, SCAN_START, SCAN_COMPLETE, SSID_ENTERED, WAIT_PASS, PASS_ENTERED, WAIT_CONNECT, LOGIN_FAILED };
enum wifi_setup_stages wifi_stage = NONE;
String network_string;
String connected_string;
String ssids_array[50];
//------------------------------------------------------

WiFiServer server(80);
BluetoothSerial SerialBT;
Preferences preferences;

void setup()
{
    Serial.begin(115200);
    pinMode(2,OUTPUT);
    preferences.begin("wifi_access",false);
    
    if(wiFiConfig()){
      bluetooth_state = false;
      Serial.println("\n");
      Serial.println("DEVICE_HOST: "+host);
      server.begin();
    }else{
      bluetoothConfig();
      bluetooth_state = true;
    }

    delay(1000);
}

void clearPreferences(){
  preferences.putString("pref_ssid", "");
  preferences.putString("pref_pass", "");
  preferences.putString("pref_host", "");
  preferences.putString("pref_ip","");
  ESP.restart();
}

void loop(){
  if(bluetooth_state)
      loopBT();
  else
    loopWiFi();
    
  loopSerial();
}


void loopSerial(){
  if(Serial.available()){
    char c = Serial.read();
    if(c == 'c'){
      Serial.println("Limpiando preferencias . . .");
      delay(1000);
      clearPreferences();
    }
  }
}
