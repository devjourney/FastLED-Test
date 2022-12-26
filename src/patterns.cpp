#include <common.h>
#include <FastLED.h>

#define TOTAL_NUM_LEDS 144
#define LED_PIN 5 // D1 on the ESP8266 E12
#define ERROR_DURATION 1000
#define PROGRESS_SECONDS 5

CRGB g_LEDs[TOTAL_NUM_LEDS] = {0};
uint8_t g_initialHue = 0;
const uint8_t g_deltaHue = 16;
const uint8_t g_hueDensity = 4;
unsigned long g_nextErrorMillis = 0;
CRGB g_errorColor = CRGB::Red;
unsigned long g_startProgressMillis = 0;

void draw_null()
{
    fill_solid(g_LEDs, TOTAL_NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void draw_error()
{
    unsigned long now = millis();
    if (now > g_nextErrorMillis)
    {
        // oscillate between red and yellow
        g_errorColor = g_errorColor.g == 0xFF ? CRGB::Red : CRGB::Yellow;
        fill_solid(g_LEDs, TOTAL_NUM_LEDS, g_errorColor);
        g_nextErrorMillis = now + ERROR_DURATION;
    }
    FastLED.show();
}

void draw_progress()
{
    double progress_percentage = min(1.0, max(0.0, (double)(millis() - g_startProgressMillis) / (double)(PROGRESS_SECONDS * 1000)));
    int maxLit = (int)floor(progress_percentage * TOTAL_NUM_LEDS);
    fill_solid(g_LEDs, TOTAL_NUM_LEDS, CRGB::Black);
    fill_solid(g_LEDs, maxLit, CRGB::Green);
    FastLED.show();
    if (progress_percentage >= 1.0)
        g_startProgressMillis = millis();
}

void draw_rainbow()
{
    fill_rainbow(g_LEDs, TOTAL_NUM_LEDS, g_initialHue += g_hueDensity, g_deltaHue);
    FastLED.show();
}

void draw_gradient()
{
    uint8_t starthue = beatsin8(5, 0, 255);
    uint8_t endhue = beatsin8(7, 0, 255);

    fill_gradient(g_LEDs, TOTAL_NUM_LEDS, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), (starthue < endhue) ? FORWARD_HUES : BACKWARD_HUES);
    FastLED.show();
}

void (*draw)() = draw_null;

void init_patterns()
{
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, TOTAL_NUM_LEDS);
    FastLED.setBrightness(5);
    draw = &draw_rainbow;
}

void draw_led()
{
    draw();
}

bool select_pattern(String name)
{
    if (name.equalsIgnoreCase("rainbow"))
        draw = &draw_rainbow;
    else if (name.equalsIgnoreCase("gradient"))
        draw = &draw_gradient;
    else if (name.equalsIgnoreCase("progress"))
    {
        g_startProgressMillis = millis();
        draw = &draw_progress;
    }
    else if (name.equalsIgnoreCase("null"))
        draw = &draw_null;
    else if (name.equalsIgnoreCase("error"))
    {
        g_nextErrorMillis = 0;
        g_errorColor = CRGB::Yellow;
        draw = &draw_error;
    }
    else
        return false;
    return true;
}
