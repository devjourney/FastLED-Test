#include <Arduino.h>

void init_patterns();
bool init_network();
void init_web();
void handle_client();
bool select_pattern(String name);
bool exec_control(String name);
void draw_led();
