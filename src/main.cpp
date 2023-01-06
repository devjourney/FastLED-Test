#include <common.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println('\n');
  init_patterns();
  if (!init_network())
  {
    Serial.println("\nStartup failed.");
    select_pattern("error");
    return;
  }
  init_web();
  Serial.println("\nStartup complete.");
  select_pattern("gradient");
}

void loop()
{
  handle_client();
  draw_led();
}
