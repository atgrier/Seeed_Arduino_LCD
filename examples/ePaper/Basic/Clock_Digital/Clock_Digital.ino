/*
 An example digital clock using a TFT LCD screen to show the time.
 Demonstrates use of the font printing routines. (Time updates but date does not.)
 
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo. 
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Updated by Bodmer
A few colour codes:
 
code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink

 */
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#ifdef EPAPER_ENABLE // Only compile this code if the EPAPER_ENABLE is defined in User_Setup.h

EPaper epaper = EPaper();  // Invoke library, pins defined in User_Setup.h

uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
bool initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

#endif

void setup(void) {
#ifdef EPAPER_ENABLE
  epaper.begin();
  epaper.setRotation(1);
  epaper.fillScreen(TFT_BLACK);

  epaper.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
  epaper.update(); // update the display
  targetTime = millis() + 1000; 
#endif
}

void loop() {
#ifdef EPAPER_ENABLE
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      omm = mm;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    if (ss==0 || initial) {
      initial = 0;
      epaper.setTextColor(TFT_GREEN, TFT_BLACK);
      epaper.setCursor (8, 52);
      epaper.print(__DATE__); // This uses the standard ADAFruit small font

      epaper.setTextColor(TFT_BLUE, TFT_BLACK);
      epaper.drawCentreString("It is windy",120,48,2); // Next size up font 2

      //epaper.setTextColor(0xF81F, TFT_BLACK); // Pink
      //epaper.drawCentreString("12.34",80,100,6); // Large font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 . : a p m
    }

    // Update digital time
    byte xpos = 6;
    byte ypos = 0;
    if (omm != mm) { // Only redraw every minute to minimise flicker
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      epaper.setTextColor(TFT_BLACK, TFT_BLACK);  // Leave a 7 segment ghost image, comment out next line!
      //epaper.setTextColor(TFT_BLACK, TFT_BLACK); // Set font colour to black to wipe image
      // Font 7 is to show a pseudo 7 segment display.
      // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      epaper.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it
      epaper.setTextColor(0xFBE0); // Orange
      omm = mm;

      if (hh<10) xpos+= epaper.drawChar('0',xpos,ypos,7);
      xpos+= epaper.drawNumber(hh,xpos,ypos,7);
      xcolon=xpos;
      xpos+= epaper.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= epaper.drawChar('0',xpos,ypos,7);
      epaper.drawNumber(mm,xpos,ypos,7);
    }

    if (ss%2) { // Flash the colon
      epaper.setTextColor(TFT_BLACK, TFT_BLACK);
      xpos+= epaper.drawChar(':',xcolon,ypos,7);
      epaper.setTextColor(0xFBE0, TFT_BLACK);
    }
    else {
      epaper.drawChar(':',xcolon,ypos,7);
      colour = random(0xFFFF);
      // Erase the old text with a rectangle, the disadvantage of this method is increased display flicker
      epaper.fillRect (0, 64, 160, 20, TFT_BLACK);
      epaper.setTextColor(TFT_WHITE);
    }
    epaper.update(); // update the display
  }
#endif
}
