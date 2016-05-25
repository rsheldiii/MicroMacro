
/* Hello and welcome to the MicroMacro software!
 * if you don't want to know how the MicroMacro works that's fine,
 * you can safely chuck this guy onto your controller and get hopping -
 * it will work just as it did in the gif. if you want it to say something
 * different, edit the const char command[] you see down below to say
 * whatever you want!
 *
 * if you do want to learn how everything works I invite you to read the comments,
 * though if you are well versed in computer science they might be a little dull.
 * I have made two modes of use - easy mode and advanced mode. Easy mode
 * allows you to specify a string that will be dutifully printed whenever you
 * click the button. Advanced mode allows you to hit most of the standard buttons
 * on a keyboard (including win, alt, function keys, etc) by specifying an array of
 * control code pairs - the modifier keys held down and the scancode of the character.
 * in order to use this you'll need to cross-reference the ascii scancode files with an 
 * ascii table, as well as read into the DigiKeyboard.h file to see the codes for the
 * modifier keys. those two files can be found here:
 * https://github.com/digistump/DigisparkArduinoIntegration/blob/master/libraries/DigisparkKeyboard/scancode-ascii-table.h
 * https://github.com/digistump/DigisparkArduinoIntegration/blob/master/libraries/DigisparkKeyboard/DigiKeyboard.h
 *
 * That's all, have fun!
 */

// include the digikeyboard library, which handles connecting as a HID device and actually emitting characters
#include <DigiKeyboard.h>

// define a few constants to make our life easier.
#define ON LOW
#define OFF HIGH
#define SCANCODE_DIFF 8 // they cut a few values from the front of the ascii table so we have to compensate
#define PAUSE_ON_NEWLINES false
#define NUM_COMMANDS 1
#define SWITCH_PIN 0
#define NO_MODS 0

int debounce = 0;

const char command[][2] = {
  {MOD_ALT_LEFT, 43},
  {MOD_ALT_LEFT, 40},
  {0, 0},
  //{MOD_ALT_LEFT, 43},
  //{0, 0}
};

void setup() {
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, HIGH);
  pinMode(SWITCH_PIN, INPUT);
}

void loop() {
  DigiKeyboard.update();

  if(digitalRead(SWITCH_PIN) == ON && debounce == 0){
    debounce = 50;
    doCommand();
  } else if (digitalRead(SWITCH_PIN) == OFF && debounce > 0){
    debounce -= 1; 
  }
}

void doCommand(){
  for (int i = 0; i < sizeof(command); i++){
    printScancode(command[i]);
    delay(100);
  }
}

void printString(const char string[]){
  for (unsigned int i = 0; i < strlen(string); i++){
    printLetter(string[i]);
  }
}

void printLetter(unsigned int letter){
  unsigned int scancode = pgm_read_byte_near(ascii_to_scan_code_table + letter - SCANCODE_DIFF);
  unsigned int keycode = scancode & 0b01111111;
  unsigned int shift = (scancode >> 7) ? MOD_SHIFT_LEFT : 0;

  if (PAUSE_ON_NEWLINES && scancode == 40){
    delay(1000);
  }

  DigiKeyboard.sendKeyStroke(keycode, shift);
}

void printScancode(const char code[2]){
  char scancode = code[1];
  char modifiers = code[0];
  
  if (PAUSE_ON_NEWLINES && scancode == 40){
    delay(1000);
  }

  DigiKeyboard.sendKeyStroke(scancode,modifiers);
}

