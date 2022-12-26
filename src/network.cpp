#include <common.h>
#include <ESP8266WiFi.h>

#define NET_TIMEOUT 10 // try to connect to WiFi for 10s

const char *ssid = "IoT";
const char *password = "change_me";
unsigned long nextInitializationCheckpoint = 0;
unsigned long initializationTimeout = 0;
uint8 initializationCount = 0;

void init_network_begin()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ...");
    Serial.print("0 ");
    nextInitializationCheckpoint = millis() + 1000;
    initializationTimeout = millis() + (NET_TIMEOUT * 1000);
}

int init_network_continue()
{
    unsigned long now = millis();
    bool connected = (WiFi.status() == WL_CONNECTED);
    if (connected)
    {
        Serial.println('\n');
        Serial.println("WiFi connection established.");
        Serial.print("Local IP:\t");
        Serial.println(WiFi.localIP());
        Serial.print("MAC Address:\t");
        Serial.println(WiFi.macAddress());
        Serial.print("DNS IP:\t");
        Serial.println(WiFi.dnsIP());
        return 1;
    }
    if (now >= initializationTimeout)
    {
        Serial.println("\nERROR: Could not connect to the network.");
        return -1;
    }
    if (now >= nextInitializationCheckpoint)
    {
        Serial.print(++initializationCount);
        Serial.print(' ');
        nextInitializationCheckpoint = millis() + 1000;
    }
    return 0;
}
