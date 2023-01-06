#include <common.h>
#include <ESP8266WebServer.h>

#define HTTP_SERVER_PORT 80

ESP8266WebServer g_webServer(HTTP_SERVER_PORT);
bool g_webInitialized = false;

void handle_root()
{
    g_webServer.send(200, "text/html", "<html><head><title>LED Control</title></head><body><h1>LED Control</h1><ul><li><a href=\"/rainbow\">Rainbow</a> - Draw a rainbow.</li><li><a href=\"/gradient\">Gradient</a> - Draw a gradient.</li><li><a href=\"/error\">Error</a> - Draw an error.</li><li><a href=\"/progress\">Progress</a> - Draw a progress bar.</li></ul></body></html>");
}

void handle_not_found()
{
    const String pattern = g_webServer.uri().substring(1);
    if (!pattern.equalsIgnoreCase("favicon.ico"))
    {
        //Serial.println(pattern);
        select_pattern(pattern);
    }
    g_webServer.sendHeader("Location", "/");
    g_webServer.send(303);
}

void init_web()
{
    g_webServer.on("/", handle_root);
    g_webServer.onNotFound(handle_not_found);
    g_webServer.begin();
    Serial.print("HTTP server started on port ");
    Serial.println(HTTP_SERVER_PORT);
    g_webInitialized = true;
}

void handle_client()
{
    if (g_webInitialized)
        g_webServer.handleClient();
}
