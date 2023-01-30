#include <common.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>

#define HTTP_SERVER_PORT 80

ESP8266WebServer g_webServer(HTTP_SERVER_PORT);
bool g_webInitialized = false;
String g_message;
String g_mainPageTemplate0 =
    "<html>"
        "<head>"
            "<title>LED Control</title>"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "</head>"
        "<body>"
            "<h1>LED Control</h1>";
String g_mainPageTemplate1 =
            "<ul>"
                "<li>Patterns"
                    "<ul>"
                        "<li><a href=\"/pattern/rainbow\">Rainbow</a></li>"
                        "<li><a href=\"/pattern/gradient\">Gradient</a></li>"
                        "<li><a href=\"/pattern/error\">Error</a></li>"
                        "<li><a href=\"/pattern/progress\">Progress</a></li>"
                        "<li><a href=\"/pattern/fire\">Fire</a></li>"
                    "</ul>"
                "</li>"
                "<li>Controls"
                    "<ul>"
                        "<li><a href=\"/control/brighten\">Brighten</a></li>"
                        "<li><a href=\"/control/dim\">Dim</a></li>"
                    "</ul>"
                "</li>"
            "</ul>"
        "</body>"
    "</html>";

String generate_page()
{
    if (g_message.isEmpty())
        return g_mainPageTemplate0 + g_mainPageTemplate1;
    return g_mainPageTemplate0 + "<h3>" + g_message + "</h3>" + g_mainPageTemplate1;
}

void handle_root()
{
    g_message = "";
    g_webServer.send(200, "text/html", generate_page());
}

void handle_not_found()
{
    g_message = "That page does not exist.";
    g_webServer.send(404, "text/html", generate_page());
}

void handle_pattern()
{
    if (select_pattern(g_webServer.pathArg(0)))
    {
        g_message = "Pattern '" + g_webServer.pathArg(0) + "' selected.";
        g_webServer.send(200, "text/html", generate_page());
    }
    else
    {
        Serial.println("Pattern '" + g_webServer.pathArg(0) + "' not found.");
        g_message = "Pattern '" + g_webServer.pathArg(0) + "' not found.";
        g_webServer.send(404, "text/html", generate_page());
    }
}

void handle_control()
{
    if (exec_control(g_webServer.pathArg(0)))
    {
        g_message = "Control '" + g_webServer.pathArg(0) + "' executed.";
        g_webServer.send(200, "text/html", generate_page());
    }
    else
    {
        Serial.println("Control '" + g_webServer.pathArg(0) + "' failed or not found.");
        g_message = "Control '" + g_webServer.pathArg(0) + "' failed or not found.";
        g_webServer.send(404, "text/html", generate_page());
    }
}

void init_web()
{
    g_webServer.on("/", handle_root);
    g_webServer.on(UriBraces("/pattern/{}"), handle_pattern);
    g_webServer.on(UriBraces("/control/{}"), handle_control);
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
