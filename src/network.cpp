#include <common.h>
#include <WiFiManager.h>

bool init_network()
{
    WiFi.mode(WIFI_STA);
    WiFiManager wifiMgr;
    Serial.println("Initializing the network...");
    if (!wifiMgr.autoConnect("LEDSETUP"))
    {
        Serial.println("ERROR: The network could not be initialized.");
        return false;
    }
    Serial.println("The network was configured.");
    return true;
}
