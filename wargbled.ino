#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <BitbangNEC.h>

#define nec_address 0xff
#define ir_led        12

// codes found here; reverse the endianness! http://blog.allgaiershops.com/2012/05/10/reversing-an-rgb-led-remote/
#define nec_power  0x20
#define nec_white  0x22
#define nec_red    0xa1
#define nec_orange 0xa0
#define nec_yellow 0x81
#define nec_diy1   0x03
#define nec_diy2   0x0b
#define nec_diy3   0x07
#define nec_diy4   0x01
#define nec_diy5   0x09
#define nec_diy6   0x05

#define the_lcd    V0
#define the_zergba V7

#define button_flux_auto  V1
#define button_manual     V2
#define button_strobe     V3
#define button_pulse      V4
#define button_submit_rgb V8
#define button_momentary  V9
#define button_kill_chip V11
#define button_power     V12

#define slider_flux_wp_override V5
#define slider_speed            V6
#define slider_bias            V10

char auth[] = "cdbe830216a6483682cd148219f7b4c4";
WidgetLCD lcd(0);

int current_mode = nec_orange;
boolean restart = false;

void beam_preserve_mode(int command) {
  ir_send_nec(nec_address, command);
}

void rebeam() {
  beam_preserve_mode(current_mode);
}

void beam(int command) {
  current_mode = command;
  beam_preserve_mode(command);
}

BLYNK_WRITE(button_power) {
  beam_preserve_mode(nec_power);
}

BLYNK_WRITE(button_kill_chip) {
  beam(nec_red);
  lcd.print(0, 0, "Restarting...");
  restart = true;
}

BLYNK_WRITE(button_momentary) {
  lcd.print(0, 1, param.asString()); //DEBUG

  if (param.asInt() == 0) {
    rebeam();
  } else {
    beam_preserve_mode(nec_white);
  }
}

void setup() {
  ir_setup(ir_led);
  beam(nec_yellow);
  Blynk.begin(auth, "revspace-pub-2.4ghz", "");
  while (!Blynk.connect()) { delay(10); }
  beam(nec_orange);
  lcd.print(0, 0, "Hello!");
  delay(500);
  lcd.clear();
  lcd.print(10, 0, "...");
}

void loop() {
  Blynk.run();
  if (restart) {
    restart = false;
    delay(500);
    ESP.restart();
  }
}
