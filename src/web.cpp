#include <common.h>
#include <ESP8266WebServer.h>

#define HTTP_SERVER_PORT 80

ESP8266WebServer web_server(HTTP_SERVER_PORT);

void handle_root()
{
    web_server.send(200, "text/html", "<html><head><title>LED Control</title></head><body><h1>LED Control</h1><ul><li><a href=\"/rainbow\">Rainbow</a> - Draw a rainbow.</li><li><a href=\"/gradient\">Gradient</a> - Draw a gradient.</li><li><a href=\"/error\">Error</a> - Draw an error.</li><li><a href=\"/progress\">Progress</a> - Draw a progress bar.</li></ul></body></html>");
}

void handle_not_found()
{
    const String pattern = web_server.uri().substring(1);
    select_pattern(pattern);
    web_server.sendHeader("Location", "/");
    web_server.send(303);
}

void init_web()
{
    web_server.on("/", handle_root);
    web_server.onNotFound(handle_not_found);
    web_server.begin();
    Serial.print("HTTP server started on port ");
    Serial.println(HTTP_SERVER_PORT);
}

void handle_client()
{
    web_server.handleClient();
}
