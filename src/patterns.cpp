#include <common.h>
#include <FastLED.h>

#define TOTAL_NUM_LEDS 144
#define LED_PIN_A 5 // GPIO5 is pin D1 on the ESP8266 E12
#define LED_PIN_B 4 // GPIO4 is pin D2 on the ESP8266 E12
#define ERROR_DURATION 1000
#define PROGRESS_SECONDS 5
#define COOLING 55
#define SPARKING 120
#define FRAMES_PER_SECOND 30

CRGB g_LEDs[TOTAL_NUM_LEDS] = {0};
uint8_t g_initialHue = 0;
const uint8_t g_deltaHue = 16;
const uint8_t g_hueDensity = 4;
unsigned long g_nextErrorMillis = 0;
CRGB g_errorColor = CRGB::Red;
unsigned long g_startProgressMillis = 0;
bool g_ReverseDirection = false;
CRGBPalette16 g_Pal;

// https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void draw_fire()
{
    random16_add_entropy(random16());

    // Array of temperature readings at each simulation cell
    static uint8_t heat[TOTAL_NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < TOTAL_NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / TOTAL_NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = TOTAL_NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < TOTAL_NUM_LEDS; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        uint8_t colorindex = scale8(heat[j], 240);
        CRGB color = ColorFromPalette(g_Pal, colorindex);
        int pixelnumber;
        if (g_ReverseDirection)
        {
            pixelnumber = (TOTAL_NUM_LEDS - 1) - j;
        }
        else
        {
            pixelnumber = j;
        }
        g_LEDs[pixelnumber] = color;
    }
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void draw_null()
{
    fill_solid(g_LEDs, TOTAL_NUM_LEDS, CRGB::Black);
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
}

void draw_progress()
{
    double progress_percentage = min(1.0, max(0.0, (double)(millis() - g_startProgressMillis) / (double)(PROGRESS_SECONDS * 1000)));
    int maxLit = (int)floor(progress_percentage * TOTAL_NUM_LEDS);
    fill_solid(g_LEDs, TOTAL_NUM_LEDS, CRGB::Black);
    fill_solid(g_LEDs, maxLit, CRGB::Green);
    if (progress_percentage >= 1.0)
        g_startProgressMillis = millis();
}

void draw_rainbow()
{
    fill_rainbow(g_LEDs, TOTAL_NUM_LEDS, g_initialHue += g_hueDensity, g_deltaHue);
}

void draw_gradient()
{
    uint8_t starthue = beatsin8(5, 0, 255);
    uint8_t endhue = beatsin8(7, 0, 255);

    fill_gradient(g_LEDs, TOTAL_NUM_LEDS, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), (starthue < endhue) ? FORWARD_HUES : BACKWARD_HUES);
}

void (*draw)() = &draw_gradient;

void init_patterns()
{
    g_Pal = PartyColors_p;
    pinMode(LED_PIN_A, OUTPUT);
    pinMode(LED_PIN_B, OUTPUT);
    FastLED.addLeds<WS2812B, LED_PIN_A, GRB>(g_LEDs, TOTAL_NUM_LEDS);
    FastLED.addLeds<WS2812B, LED_PIN_B, GRB>(g_LEDs, TOTAL_NUM_LEDS);
    FastLED.setBrightness(100);
}

void draw_led()
{
    random16_add_entropy(random16());
    draw();
    FastLED.show();
}

bool select_pattern(String name)
{
    if (name.equalsIgnoreCase("rainbow"))
        draw = &draw_rainbow;
    else if (name.equalsIgnoreCase("gradient"))
        draw = &draw_gradient;
    else if (name.equalsIgnoreCase("fire"))
        draw = &draw_fire;
    else if (name.equalsIgnoreCase("progress"))
    {
        g_startProgressMillis = millis();
        draw = &draw_progress;
    }
    else if (name.equalsIgnoreCase("null"))
        draw = &draw_null;
    else
    {
        g_nextErrorMillis = 0;
        g_errorColor = CRGB::Yellow;
        draw = &draw_error;
        if (!name.equalsIgnoreCase("error"))
            return false;
    }
    return true;
}
