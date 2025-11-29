#include <HTTPClient.h>
#include <WiFiClientSecure.h>

WiFiClientSecure wifiClient;
HTTPClient http;

//http(s)://$THINGSBOARD_HOST_NAME/api/v1/$ACCESS_TOKEN/telemetry
String serverName = "https://thingcontrol.io/api/v1/";

void httpSend(String msg){

    wifiClient.setInsecure();
    http.begin(wifiClient, serverName);

    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(msg);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
    
}