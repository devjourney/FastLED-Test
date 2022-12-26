#include <common.h>

enum Status
{
  in_error,
  network_initializing,
  running
};
Status state = network_initializing;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println('\n');
  init_patterns();
  select_pattern("progress");
  state = network_initializing;
  init_network_begin();
}

void loop()
{
  switch (state)
  {
    default:
    case in_error:
      break;
    case network_initializing:
      switch (init_network_continue())
      {
        default:
        case -1:
          // network could not be connected
          select_pattern("error");
          state = in_error;
          break;
        case 0:
          // network initialization continues
          break;
        case 1:
          // network initialization finished successfully
          init_web();
          Serial.println("\nStartup complete.");
          select_pattern("gradient");
          state = running;
          break;
      }
      break;
    case running:
      handle_client();
      break;
  }
  draw_led();
}
