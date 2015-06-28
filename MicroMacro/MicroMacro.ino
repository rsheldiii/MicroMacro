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
#define LED 1
#define SWITCH 0
#define SCANCODE_DIFF 8 // they cut a few values from the front of the ascii table so we have to compensate
#define PAUSE_ON_NEWLINES 1

/*
 * this is our macro string, which we send to the printString() function
 * so it can emit the command as if it were a keyboard. 
 * you can write whatever you want in here and it should be able to handle it.
 * note to the industrious, you can't get the DigiKeyboard.write() function to do
 * things like function keys or the delays you saw in the gif - it merely runs off
 * the ascii scan code table linked above.
 * if you want to send stuff like function keys you'll have to use a loop and call
 * the sendKeyStroke function yourself. I implemented that in 
 */
const char command[] = "Son,\nThe internet is broken and it is your fault\nPlease fix the internet or you are grounded\nLove, Mom";

/*
 * this is the advanced settings scancode array.
 * currently all it does it alt+tab, because that's actually super useful and neat,
 * but you could string together however many commands you wanted!
 * the array is actually an array of arrays - the arrays inside of it
 * are all 2 elements long, and the first element is always the modifier mask,
 * with the second element being the scancode of the character you want to emit.
 * this means if you want multiple modifiers pressed at the same time, you'll need
 * to or them together, so MOD_CONTROL_LEFT | MOD_SHIFT_LEFT for instance.
 * modifier scancodes and an ascii conversion table are in the above linked documents.
 */
char scancodeArr[][2] = { 
    {MOD_ALT_LEFT,43}
};

// this boolean keeps track of whether the button has been clicked.
// this is helpful for not sending multiple clicks
bool isPressed = false;


// this function is arduino-specific, and is run once when the device starts your program.
// it is used to do any necessary setup, like defining what pins do what
void setup() {
  /* 
   * cherry switches are active-low, meaning when you click on the switch,
   * it ties the pin to ground and the pin reads 0. Unfortunately the
   * DigiSpark pins start out low, so we tie them to high first so they
   * work correctly
   */
  pinMode(SWITCH, OUTPUT);
  digitalWrite(SWITCH, HIGH);
  
  // let's set the other LED to HIGH for MAXIMUM BACKLIGHTING
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // switch this to LOW or uncomment this line if you only want the other LED to light up when the switch is held down

  // now we tell the digispark the switch pin is an input, not an output
  pinMode(SWITCH, INPUT);
  
}

// this function is arduino specific, and is run over and over again until the device is unplugged
// after the setup() function finishes.
void loop() {
  // this function sends a ping to the computer so that it knows the keyboard isn't dead
  DigiKeyboard.update();

  // if the switch is pressed down and we haven't held onto it since last time, print the macro
  if(digitalRead(SWITCH) == ON && !isPressed){
    //digitalWrite(LED,HIGH); // uncomment if you only want the status LED to light up when the switch is held down
    isPressed = true;
    
    // these commands are the ones that actually send the keystroke to the computer
    printString(command); // switch these two statements if you want to use advanced mode
    //printScancode(scancodeArr);
  }
  
  /*
   * if the switch has just turned off we want to do what's called
   * "debouncing", which helps to make sure you don't get multiple
   * button presses. This won't matter for a long string, but if
   * you have a single character it definitely makes a difference
   */
  if (digitalRead(SWITCH) == OFF && isPressed){
    //digitalWrite(LED, LOW); // uncomment if you only want the status LED to light up when the switch is held down
    isPressed = false; 
    delay(5); // debounce
  }
}

// this is our own little function to print strings.
// we use this instead of DigiKeyboard.write so we can do fun stuff like pause on newlines
void printString(const char string[]){
  for (unsigned int i = 0; i < strlen(string); i++){
    printLetter(string[i]);
  }
}

// this function does the heavy lifting for turning the ascii into a modifier mask and scancode
// with the help of the ascii_to_scan_code_table
void printLetter(unsigned int letter){
  // the ascii scancode table is saved in program memory, which means we have to use pgm_read_byte_near
  // to access it. which was news to me I'll tell you that much
  unsigned int scancode = pgm_read_byte_near(ascii_to_scan_code_table + letter - SCANCODE_DIFF);
  
  // the last bit in the scancode tells us if the character is shifted or not, so
  // we use what's called a "mask" to get rid of that bit to know what unshifted code to send
  unsigned int keycode = scancode & 0b01111111;
  
  // we use "bit shifting" to get rid of anything but the bit that tells us if the shift button
  // should be pressed - then we use a ternary operator to either display the correct code
  // for pressing the shift button or 0 - the code for no modifiers
  unsigned int shift = (scancode >> 7) ? MOD_SHIFT_LEFT : 0;

  // here is where we check if we want to pause for newlines
  if (PAUSE_ON_NEWLINES && scancode == 40){
    delay(1000);
  }

  DigiKeyboard.sendKeyStroke(keycode, shift);
}

// advanced function for using modifiers and direct scancodes
// there's almost no logic in here so I didn't write any comments...
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

