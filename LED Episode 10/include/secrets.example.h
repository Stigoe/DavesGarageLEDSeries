//+--------------------------------------------------------------------------
//
// File:        secrets.h
//
//
// Description:
//
//    secret definitions, at the moment this is just network secrets, 
//    but could be expanded in the future
//
//---------------------------------------------------------------------------

// NOTE: do NOT enter your network details in this file (secrets.example.h)! 
// Instead, copy this file to secrets.h, and set the below defines in that file!

#define ssid      "Your SSID"
#define pass      "Your PASS"
#define Hostname  "ESP32Client"

// define the MQTT server to connect to
#define mqtt_server  "your MQTT server IP address"
#define mqtt_user    "mqtt-user"
#define mqtt_pass    "mqtt-pass"
