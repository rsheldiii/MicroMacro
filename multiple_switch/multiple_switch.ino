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
#define PAUSE_ON_NEWLINES 1
#define NUM_COMMANDS 2

// list of pins that switches are wired to
const int switchList[NUM_COMMANDS] = {0,2};

// list of string commands that switches have. note that the order is the same as above; pin 0 has "a"
const char* commandList[NUM_COMMANDS] = {"a","d"};

// advanced version only
/*char scancodeArr[NUM_COMMANDS][2] = { 
    {MOD_ALT_LEFT,43},
    {MOD_ALT_LEFT,43}
};*/

//debounce array. when 0, keys can be pressed. allows keys to debounce separately, so speed is less limited
int debounce[NUM_COMMANDS] = {0,0};

void setup() {
  // set all pins we are checking for input to high
  for(int i = 0; i < NUM_COMMANDS/*sizeof(commandList)*/; i++){
    pinMode(switchList[i], OUTPUT);
    digitalWrite(switchList[i], HIGH);
    pinMode(switchList[i], INPUT);
  }
}

void loop() {
  DigiKeyboard.update();

  for (int i = 0; i < NUM_COMMANDS; i++){
    if(digitalRead(switchList[i]) == ON && debounce[i] == 0){
      debounce[i] = 50;
      printString(commandList[i]);
      //printScancode(scancodeArr);
    } else if (digitalRead(switchList[i]) == OFF && debounce[i] > 0){
      debounce[i] -= 1; 
    }
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

void printScancode(char codes[][2]){
  for (unsigned int i = 0; i < sizeof(codes); i++){
    char* codepoint = codes[i];
    char scancode = codepoint[1];
    char modifiers = codepoint[0];
    
    if (PAUSE_ON_NEWLINES && scancode == 40){
      delay(1000);
    }
    
    DigiKeyboard.sendKeyStroke(scancode,modifiers);
  }
}

